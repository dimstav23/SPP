#include <llvm/Transforms/Utils/Local.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

#include <set>
#include <map>

#define BOUND_SHIFT 32 // TODO: add compiler opt for this

/*
 * Get the insert point after the specified instruction. For non-terminators
 * this is the next instruction. For `invoke` intructions, create a new
 * fallthrough block that jumps to the default return target, and return the
 * jump instruction.
 */
Instruction* TagProp::getInsertPointAfter(Instruction* I) {
    if (InvokeInst* Invoke = dyn_cast<InvokeInst>(I)) {
        BasicBlock* Dst = Invoke->getNormalDest();
        BasicBlock* NewBlock = BasicBlock::Create(I->getContext(),
            "invoke_insert_point", Dst->getParent(), Dst);
        BranchInst* Br = BranchInst::Create(Dst, NewBlock);
        Invoke->setNormalDest(NewBlock);

        /* Patch references in PN nodes in original successor */
        BasicBlock::iterator It(Dst->begin());
        while (PHINode* PN = dyn_cast<PHINode>(It)) {
            int i;
            while ((i = PN->getBasicBlockIndex(Invoke->getParent())) >= 0)
                PN->setIncomingBlock(i, NewBlock);
            It++;
        }

        return Br;
    }

    if (isa<PHINode>(I))
        return &*I->getParent()->getFirstInsertionPt();

    assert(!isa<TerminatorInst>(I));
    return &*std::next(BasicBlock::iterator(I));
}


/*
 * On pointer arithmetic, replicate the operations on the metadata in upper
 * bits.
 */
bool TagProp::instrumentPtrArith(GetElementPtrInst* Gep) {
    GetElementPtrInst* PreemptedGep = nullptr;

    if (!PreemptedGep) {
        /* No effect on ptr means no effect on size. */
        if (Gep->hasAllZeroIndices())
            return false;

        /* We want to skip GEPs on vtable stuff, as they shouldn't be able to
         * overflow, and because they don't have metadata normally negative
         * GEPs fail on these. */
        if (isVtableGep(Gep))
            return false;
    }

    /* TODO: we cannot support GEPs operating on vectors. */
    if (Gep->getType()->isVectorTy()) {
        LOG_LINE("Warning: ignoring GEP on vector: " << *Gep);
        return false;
    }

    std::string Prefix = Gep->hasName() ? Gep->getName().str() + "." : "";
    IRBuilder<> B(getInsertPointAfter(Gep));
    std::vector<User*> Users(Gep->user_begin(), Gep->user_end());
    IntegerType* PtrIntTy = getPtrIntTy(Gep->getContext());

    /* NOTE: further optimization: if only last index non-zero we can create a
     * new GEP instead of all below, which may be better for optimizer? */

    Instruction* PtrInt = cast<Instruction>(B.CreatePtrToInt(Gep, PtrIntTy, Prefix + "int"));

    /* Generate calculation of offset (for every idx, multiply element size by
     * element idx, and add all together). IRBuilder does proper constant
     * folding on this, meaning that if the entire offset is known at compile
     * time, no calculation will be present in IR. */
    Value* Diff;
    ConstantInt* ConstOffset = nullptr;

    APInt ConstOffsetVal(PointerBits, 0);
    if (Gep->accumulateConstantOffset(*DL, ConstOffsetVal))
        ConstOffset = B.getInt(ConstOffsetVal);

    if (PreemptedGep) {
        APInt PreemptedOffset(PointerBits, 0);
        if (PreemptedGep->accumulateConstantOffset(*DL, PreemptedOffset)) {
            Diff = ConstantInt::getSigned(PtrIntTy, PreemptedOffset.getSExtValue());
        }
        else {
            // Move up instructions that are needed for the merged offset
            // calculation but are defined later than the GEP that does the check
            if (moveUpOffsetInsts(Gep, PreemptedGep, PtrInt)) {
                NMovedOffsets++;

                // Offset instructions are inserted between the gep and the gep's
                // ptrint so that we retreive the insertion point after the
                // offsets. Set the insertion point and move the ptrint definition
                // to directly after the gep for readaibility.
                B.SetInsertPoint(getInsertPointAfter(PtrInt));
                PtrInt->removeFromParent();
                PtrInt->insertAfter(Gep);
            }
            Diff = EmitGEPOffset(&B, *DL, PreemptedGep);
        }
    }
    else if (ConstOffset) {
        Diff = ConstOffset;
    }
    else if (SubtractionArith) {
        Value* Base = Gep->getPointerOperand();
        Value* BaseInt = B.CreatePtrToInt(Base, PtrIntTy, Prefix + "baseint");
        Diff = B.CreateSub(PtrInt, BaseInt, Prefix + "diff");
    }
    else {
        Diff = EmitGEPOffset(&B, *DL, Gep);
    }

    Value* Shifted = B.CreateShl(Diff, BOUND_SHIFT, Prefix + "shifted");
    Value* AddOffset = Shifted;
    Value* PtrAdd;
    Constant* ZeroPtr = B.getIntN(PointerBits, 0);

    /* For known negative offsets, insert a check if the pointer indeed has
     * metadata, and don't do a zero metadata addition if this is the case. */
    if (CheckUnderflow && hasNegativeOffset(Gep)) {
        // TODO: don't insert check if pointer operand certainly has metadata
        //       (if we can find the tag, i.e., if it is or'ed with a const)

        // meta = ptr >> BOUND_SHIFT  // XXX mask away overflow bit here?
        // hasmeta = meta != 0
        //Value *Meta = B.CreateLShr(PtrInt, BOUND_SHIFT, Prefix + "meta");
        //Value *HasMeta = B.CreateICmpNE(Meta, Zero, Prefix + "hasmeta");

        // hasmeta = ptr > ADDRSPACE_MASK
        // addoffset = hasmeta ? (offset << BOUND_SHIFT) : 0  // select
        Value* Zero = ConstantInt::get(PtrIntTy, 0);
        Value* Mask = ConstantInt::get(PtrIntTy, getAddressSpaceMask());
        Value* OrigPtrInt = B.CreatePtrToInt(Gep->getOperand(0), PtrIntTy, Prefix + "origptrint");
        Value* HasMeta = B.CreateICmpUGT(OrigPtrInt, Mask, Prefix + "hasmeta");
        AddOffset = B.CreateSelect(HasMeta, Shifted, Zero, Prefix + "offset");
        PtrAdd = B.CreateAdd(PtrInt, AddOffset, Prefix + "added");
        ++NUnderflowCheck;
    }
    /* For positive GEPs, replace the GEP with a nullptr if the carry flag is
     * set after the operation.
     * For dynamic GEPs, check if the offset is positive and if the operation
     * overflows. */
    else if (CheckOverflow != nocheck && !(ConstOffset && ConstOffset->isNegative())) {
        Value* OAdd = B.CreateCall(AddWithOverflowFunc, { PtrInt, AddOffset }, Prefix + "oadd");
        Value* Result = B.CreateExtractValue(OAdd, 0, Prefix + "added");
        Value* Overflow = B.CreateExtractValue(OAdd, 1, Prefix + "overflow");
        Value* NotNegativeAndOverflow = Overflow;
        if (!ConstOffset) {
            Value* Positive = B.CreateICmpSGT(Diff, ZeroPtr, Prefix + "positive");
            NotNegativeAndOverflow = B.CreateAnd(Positive, Overflow, Prefix + "both");
            ++NDynamicOverflowCheck;
        }

        switch (CheckOverflow) {
            /* Branch to trap code if the operation overflows */
        case branch: {
            // Split on condition
            BasicBlock* BB = Gep->getParent();
            BasicBlock* Succ = BB->splitBasicBlock(B.GetInsertPoint(), Prefix + "fallthru");

            // Replace unconditional jump with conditional branch
            BB->getTerminator()->eraseFromParent();
            B.SetInsertPoint(BB);
            B.CreateCondBr(NotNegativeAndOverflow, getOrCreateErrorBlock(BB->getParent()), Succ);

            // Reset insert point
            B.SetInsertPoint(&*Succ->begin());

            PtrAdd = Result;
            break;
        }
                   /* Nullify the result if the operation overflows */
        case satarith:
            PtrAdd = B.CreateSelect(NotNegativeAndOverflow, ZeroPtr, Result, Prefix + "added");
            break;
        case nocheck:
            break;
        }

        ++NOverflowCheck;
    }
    /* Default: add the offset to the metadata bits */
    else {
        PtrAdd = B.CreateAdd(PtrInt, AddOffset, Prefix + "added");
        ++NNoCheck;
    }

    // TODO: try to make the final ptr, instead of the offset, a select inst
    // (and measure performance for both)
    Value* NewPtr = B.CreateIntToPtr(PtrAdd, Gep->getType(), Prefix + "newptr");
    ++NGep;

    // TODO: check if this is optimized in asm

    for (User* U : Users)
        U->replaceUsesOfWith(Gep, NewPtr);

    // Maintain mapping for instrumentDeref
    ReplacedPtrArithReverse[NewPtr] = Gep;

    return true;
}

class TagProp : public FunctionPass {
public:
    static char ID;

    TagProp() : FunctionPass(ID) { }

    bool runOnFunction(Function& F) {
        bool Changed = false;
        SmallVector<GetElementPtrInst*, 8> Geps;
        //SmallVector<Instruction*, 8> Derefs;
        //SmallVector<Instruction*, 8> MemIntrinsics;

        //DT = &getAnalysis<DominatorTreeWrapperPass>(F).getDomTree();
        //ReplacedPtrArithReverse.clear();
        //DerefBytes.clear();

        for (Instruction& I : instructions(F)) {
            Changed |= propagatePtrMetadata(&I);

            if (EnablePtrArith) {
                ifcast(GetElementPtrInst, Gep, &I)
                    Geps.push_back(Gep);
            }

            /*if (DisallowUnalignedAccess) {
                if (isa<LoadInst>(I) || isa<StoreInst>(I))
                    Derefs.push_back(&I);
            }

            if (EnableMemIntrinsics) {
                ifcast(MemIntrinsic, MI, &I)
                    MemIntrinsics.push_back(MI);
                ifcast(CallInst, CI, &I) {
                    Function* CF = CI->getCalledFunction();
                    if (CF && CF->hasName() && CF->getName() == "memcmp")
                        MemIntrinsics.push_back(CI);
                }*/
            }
        }

        // Save preempted alignment sizes for instrumentDeref
  /*      if (SafeAlloc && !Derefs.empty()) {
            for (GetElementPtrInst* Gep : Geps) {
                if (GetElementPtrInst* PreemptedGep = SafeAlloc->getPreemptedOffset(Gep))
                    DerefBytes[Gep] = getSmallestDerefSize(PreemptedGep);
            }
        }
  */
        for (GetElementPtrInst* Gep : Geps)
            Changed |= instrumentPtrArith(Gep);

 /*       for (Instruction* I : Derefs)
            Changed |= instrumentDeref(I);

        for (Instruction* I : MemIntrinsics)
            Changed |= instrumentMemIntrinsic(I);*/

        return Changed;
    }
};

char TagProp::ID = 0;
static RegisterPass<TagProp> X("spp", "Tag Propagation Pass");