//===----- spp.cpp - Bounds Checker in SPP transformation pass -----===//
#define DEBUG_TYPE "spp"

#include <llvm/Pass.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/Dominators.h>
#include <llvm/ADT/DepthFirstIterator.h>
#include <llvm/ADT/SmallSet.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/IR/MDBuilder.h>
#include <llvm/IR/Metadata.h>
#include <llvm/Analysis/MemoryBuiltins.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/ScalarEvolutionExpressions.h>
#include <llvm/Analysis/AssumptionCache.h>
#include <llvm/Analysis/LoopAccessAnalysis.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopIterator.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/Transforms/Utils/Local.h>

#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <tr1/memory>
#include <tr1/tuple>
#include <assert.h>

using namespace llvm;

namespace {

    class SPPPass {
        Module* M = nullptr;
        TargetLibraryInfo* TLI = nullptr;
        ScalarEvolution* SE = nullptr;

        const DataLayout *DL;

        SmallSet<Function*, 32> varargFuncs;
        DenseMap<Constant*, Constant*> globalUses;
        DenseMap<Instruction*, Instruction*> optimizedMemInsts;
        SmallSet<Function*, 32> externalFuncs;
        SmallSet<Value*, 32> pmemPtrs;
        
    public:
    
        SPPPass(Module* M) {
            this->M = M;
        }

        void setScalarEvolution(ScalarEvolution* SE) {
            this->SE = SE;
        }

        void setDL(const DataLayout *DL) {
            this->DL = DL;
        }

        //TODO
        void visitGlobals() {
            SmallVector<GlobalVariable*, 16> globals;
            for (auto G = M->global_begin(), Gend = M->global_end(); G != Gend; ++G) {
                globals.push_back(&*G);
            }
        }

        /*
        * Get the insert point after the specified instruction. For non-terminators
        * this is the next instruction. For `invoke` intructions, create a new
        * fallthrough block that jumps to the default return target, and return the
        * jump instruction.
        */
        Instruction *getInsertPointAfter(Instruction *I) {
            if (InvokeInst *Invoke = dyn_cast<InvokeInst>(I)) {
                BasicBlock *Dst = Invoke->getNormalDest();
                BasicBlock *NewBlock = BasicBlock::Create(I->getContext(),
                        "invoke_insert_point", Dst->getParent(), Dst);
                BranchInst *Br = BranchInst::Create(Dst, NewBlock);
                Invoke->setNormalDest(NewBlock);

                /* Patch references in PN nodes in original successor */
                BasicBlock::iterator It(Dst->begin());
                while (PHINode *PN = dyn_cast<PHINode>(It)) {
                    int i;
                    while ((i = PN->getBasicBlockIndex(Invoke->getParent())) >= 0)
                        PN->setIncomingBlock(i, NewBlock);
                    It++;
                }
                return Br;
            }
            if (isa<PHINode>(I))
                return &*I->getParent()->getFirstInsertionPt();
           
            assert(!Instruction::isTerminator(I->getOpcode()));
            return &*std::next(BasicBlock::iterator(I));
        }

        bool instrGep(GetElementPtrInst* Gep) {
            /* No effect on ptr means no effect on size. */
            if (Gep->hasAllZeroIndices()) {
                return false;
            }
                    
            /* We want to skip GEPs on vtable stuff, as they shouldn't be able to
            * overflow, and because they don't have metadata normally negative
            * GEPs fail on these. */
            /*
            if (isVtableGep(Gep))
                return false;
            */

            /* TODO: we cannot support GEPs operating on vectors. */
            if (Gep->getType()->isVectorTy()) {
                errs() << "Warning: ignoring GEP on vector: " << *Gep << "\n";
                return false;
            }

            IRBuilder<> B(getInsertPointAfter(Gep));
            std::vector<User*> Users(Gep->user_begin(), Gep->user_end());

            //get the GEP offset
            Value *GepOff = EmitGEPOffset(&B, *DL, Gep);
            
            //extract the actual GEP offset as an 64-bit integer
            uint64_t constIntValue = 0;
            if (ConstantInt* CI = dyn_cast<ConstantInt>(GepOff)) {
                if (CI->getBitWidth() <= 64) {
                    constIntValue = CI->getSExtValue();
                }
            }

            errs() << "gep ops : ";
            Gep->print(errs());
            errs() << " || offset : " << constIntValue << "\n";
            
            /*
            for (auto Op = I.op_begin(), OpEnd = I.op_end(); Op != OpEnd; ++Op) {
                if (auto* Ptr = dyn_cast<Value>(Op)) {
                    //errs() << Ptr->getName() << ",";
                    
                    if (pmemPtrs.contains(Ptr)) {
                        I.print(errs());
                        errs() << "\n";
                        continue;
                    }
                    
                }
                        
            }
            */
            return true;
        }

        bool visitFunc(Function* F) {
            bool Changed = false;
            for (auto &I : instructions(F)) {
                /* Function calls handling --- Mask the ptr for external function calls */
                if (auto *CB = dyn_cast<CallBase>(&I)) { 
                    if (externalFuncs.contains(CB->getCalledFunction()) ) {
                        for (auto Arg = CB->arg_begin(), ArgEnd = CB->arg_end(); Arg != ArgEnd; ++Arg) {
                            if (auto* ArgVal = dyn_cast<Value>(Arg)) {
                                if (ArgVal->getType()->isPointerTy()) {
                                    IRBuilder<> B(&I);
                                    Value *Unmasked = B.CreatePtrToInt(ArgVal, B.getInt64Ty(), "unmasked_arg"); //convert to 64bit int
                                    //Value *Masked = B.CreateAnd(Unmasked, 0x7FFFFFFFFF,"masked"); // mask the 39 LSbits
                                    Value *Masked = B.CreateAnd(Unmasked, 0xFFFFFFFFFFFF,"masked_arg"); // mask the 48 LSbits
                                    Value *NewArgVal = B.CreateIntToPtr(Masked, ArgVal->getType(), "new_arg_ptr"); //convert back to ptr
                                    CB->setArgOperand(Arg - CB->arg_begin(), NewArgVal);
                                }
                                else if (auto *Gep = dyn_cast<GetElementPtrInst>(ArgVal)) {
                                    errs() << "WARNING: inserted GEP handling from function argument\n";
                                    Changed = instrGep(Gep);
                                }
                            }  
                        }
                        Changed = true;  
                    }
                }
                /* GEPs handling --- Apply the arithmetic to the top tag part*/
                else if (auto *Gep = dyn_cast<GetElementPtrInst>(&I)) {
                    Changed = instrGep(Gep);
                }
            }
            return Changed;
        }

        void addExternalFunc(Function* F) {
            externalFuncs.insert(F);
        }

        void trackPmemPtrs(Function* F) {
            for (auto &I : instructions(F)) {
                if (auto *CB = dyn_cast<CallBase>(&I)) {
                    if (CB->getCalledFunction()->getName()=="pmemobj_direct_inline") {
                        Value *Pmem_ptr = cast<Value>(&I);
                        pmemPtrs.insert(Pmem_ptr);                       
                    }
                }
            }
        }
        
    };

    class SPPModule : public ModulePass {
    public:
        static char ID;

        SPPModule() : ModulePass(ID) { }

        virtual bool runOnModule(Module& M) {
            SPPPass Spp(&M);

            Spp.setDL(&M.getDataLayout()); //init the data layout

            bool Changed = false;
            //Track the external functions first &
            //Track the pointers derived from pmemobj_direct_inline
            for (auto F = M.begin(), Fend = M.end(); F != Fend; ++F) {
                if (F->isDeclaration()) {
                    Spp.addExternalFunc(&*F);
                }
                else {
                    Spp.trackPmemPtrs(&*F);
                }
            }

            //Visit the functions to clear the appropriate ptr before external calls
            for (auto F = M.begin(), Fend = M.end(); F != Fend; ++F) {
                if (!F->isDeclaration()) {
                    Changed = Spp.visitFunc(&*F);
                }
            }
            return Changed;
        }
    };

    char SPPModule::ID = 0;
    static RegisterPass<SPPModule> X("spp", "Safe Persistent Pointers Pass", false, false);

    static void registerPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
        PM.add(new SPPModule());
    }

    //apply the module pass at this phase because EarlyAsPossible can cause UB
    static RegisterStandardPasses
    RegisterMyPass(PassManagerBuilder::EP_ModuleOptimizerEarly,
                   registerPass);

    //to keep the pass available even in -O0
    static RegisterStandardPasses
    RegisterMyPass_non_opt(PassManagerBuilder::EP_EnabledOnOptLevel0,
                   registerPass);


    class SPPTagCleaningFunc : public FunctionPass {
    public:
        static char ID;
        SPPTagCleaningFunc() : FunctionPass(ID) { }

        virtual bool runOnFunction(Function &F) {
            /* Ignore declarations */
            if (F.isDeclaration()) return false;
            bool Changed = false;
            for (auto &I : instructions(F)) {
                /* Load/Store instructions handling --- mask the ptr */
                /* should normally call the hook function for the overflow check */
                if (isa<LoadInst>(I) || isa<StoreInst>(I)) {
                    for (auto Op = I.op_begin(), OpEnd = I.op_end(); Op != OpEnd; ++Op) {
                        if (auto* Ptr = dyn_cast<Value>(Op)) {
                            if (Ptr->getType()->isPointerTy()) {
                                IRBuilder<> B(&I);
                                Value *Unmasked = B.CreatePtrToInt(Ptr, B.getInt64Ty(), "unmasked_ptr"); //convert to 64bit int
                                //Value *Masked = B.CreateAnd(Unmasked, 0x7FFFFFFFFF,"masked"); // mask the 39 LSbits
                                Value *Masked = B.CreateAnd(Unmasked, 0xFFFFFFFFFFFF,"masked_ptr"); // mask the 48 LSbits
                                Value *NewPtr = B.CreateIntToPtr(Masked, Ptr->getType(), "new_ptr"); //convert back to ptr
                                I.setOperand(Op->getOperandNo(), NewPtr);
                                Changed = true;
                            }
                        }
                    }
                }
            }
            return Changed;
        }
    };

    char SPPTagCleaningFunc::ID = 0;
    static RegisterPass<SPPTagCleaningFunc> Y("spp_tag_cleaning", "Safe Persistent Pointers Tag Cleaning Pass", false, false);

    static void registerPass_tag_cleaning(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
        PM.add(new SPPTagCleaningFunc());
    }

    static RegisterStandardPasses
    RegisterMyPass_tag_clean(PassManagerBuilder::EP_EarlyAsPossible,
                   registerPass_tag_cleaning);

    //to keep the pass available even in -O0
    static RegisterStandardPasses
    RegisterMyPass_tag_clean_non_opt(PassManagerBuilder::EP_EnabledOnOptLevel0,
                   registerPass_tag_cleaning);
}
