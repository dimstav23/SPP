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

        SmallSet<Function*, 32> varargFuncs;
        DenseMap<Constant*, Constant*> globalUses;
        DenseMap<Instruction*, Instruction*> optimizedMemInsts;
        //DenseMap<Instruction*, LowerUpperBound> boundsForInsts;
        
    public:

        //SPPPass(Module* M, TargetLibraryInfo* TLI) {
        //  this->M = M;
        //  this->TLI = TLI;
        //}

        SPPPass(Module* M) {
            this->M = M;
        }

        void setScalarEvolution(ScalarEvolution* SE) {
            this->SE = SE;
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

        void visitFunc(Function* F) {
            for (auto &I : instructions(F)) {
                if (auto *CB = dyn_cast<CallBase>(&I)) {
                    if (CB->getCalledFunction()->getName()=="pmemobj_direct_inline") {
                        
                        Value *Pmem_ptr = cast<Value>(&I); //get the return value of the pmemobj_direct_inline                  
                        std::vector<User*> Users(Pmem_ptr->user_begin(), Pmem_ptr->user_end());

                        IRBuilder<> B(SPPPass::getInsertPointAfter(&I)); //find the appropriate instert point
                    
                        Value *Unmasked = B.CreatePtrToInt(Pmem_ptr, B.getInt64Ty(), "unmasked"); //convert to 64bit int
                        //Value *Masked = B.CreateAnd(Unmasked, 0x7FFFFFFFFF,"masked"); // mask the 39 LSbits
                        Value *Masked = B.CreateAnd(Unmasked, 0xFFFFFFFFFFFF,"masked"); // mask the 48 LSbits
                        Value *New_pmem_ptr = B.CreateIntToPtr(Masked, Pmem_ptr->getType(), "new_pm_ptr"); //convert back to ptr

                        //replace all the uses
                        for (User *U : Users) {
                            U->replaceUsesOfWith(Pmem_ptr, New_pmem_ptr);
                            errs() << "replaced " << Pmem_ptr->getName() << " with " << New_pmem_ptr->getName() << "\n";
                        }                         
                    }
                    else if (CB->getCalledFunction()->getName()=="test_func") {
                        Value *test = cast<Value>(&I); //get the return value of the test_func                  
                        std::vector<User*> Users(test->user_begin(), test->user_end());

                        IRBuilder<> B(SPPPass::getInsertPointAfter(&I)); //find the appropriate instert point
                        Value *new_test = B.CreateAdd(test, ConstantInt::get(B.getInt32Ty(), 1),"new_test");
                        //replace all the uses
                        for (User *U : Users) {
                            U->replaceUsesOfWith(test, new_test);
                            errs() << "replaced " << test->getName() << " with " << new_test->getName() << "\n";
                        }                    
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
            for (auto F = M.begin(), Fend = M.end(); F != Fend; ++F) {
                if (F->isDeclaration()) continue;
                Spp.visitFunc(&*F);
            }
            return true;
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
            
            for (auto &I : instructions(F)) {
                if (isa<LoadInst>(I)) {
                    /* Load instructions handling */
                    Value *LoadPtr = I.getOperand(0);
                    IRBuilder<> B(&I);
                    Value *Unmasked = B.CreatePtrToInt(LoadPtr, B.getInt64Ty(), "unmasked_ld"); //convert to 64bit int
                    //Value *Masked = B.CreateAnd(Unmasked, 0x7FFFFFFFFF,"masked"); // mask the 39 LSbits
                    Value *Masked = B.CreateAnd(Unmasked, 0xFFFFFFFFFFFF,"masked_ld"); // mask the 48 LSbits
                    Value *NewLoadPtr = B.CreateIntToPtr(Masked, LoadPtr->getType(), "new_ld_ptr"); //convert back to ptr
                    I.setOperand(0, NewLoadPtr);
                }
                else if (isa<StoreInst>(I)) {
                    /* Store instructions handling */
                    Value *StorePtr = I.getOperand(1);
                    IRBuilder<> B(&I);
                    Value *Unmasked = B.CreatePtrToInt(StorePtr, B.getInt64Ty(), "unmasked_st"); //convert to 64bit int
                    //Value *Masked = B.CreateAnd(Unmasked, 0x7FFFFFFFFF,"masked"); // mask the 39 LSbits
                    Value *Masked = B.CreateAnd(Unmasked, 0xFFFFFFFFFFFF,"masked_st"); // mask the 48 LSbits
                    Value *NewStorePtr = B.CreateIntToPtr(Masked, StorePtr->getType(), "new_st_ptr"); //convert back to ptr
                    I.setOperand(1, NewStorePtr);
                }
            }
            return true;
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

}
