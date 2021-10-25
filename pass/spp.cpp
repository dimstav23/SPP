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
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/Dominators.h>
#include <llvm/ADT/DepthFirstIterator.h>
#include <llvm/ADT/SmallSet.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
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

            //for ()
        }

        void visitFunc(Function* F) {
            dbgs() << "Working on Function: " << F->getName() << "\n";

            for (auto BB = F->begin(), BBend = F->end(); BB != BBend; ++BB)
                for (auto I = BB->begin(), Iend = BB->end(); I != Iend; ++I) {
                    if (auto* CB = dyn_cast<CallBase>(I)) {
                        if (CB->getCalledFunction()->getName() == "pmemobj_direct_inline") {
                            errs() << "Found pmemobj";

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
            //errs() << "Running Pass";
            //TargetLibraryInfo* TLI = &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();
            SPPPass Spp(&M);

            //for (auto F = M.begin(), Fend = M.end(); F != Fend; ++F) {
            //  if (F->isDeclaration()) continue;
            //  SPP.findHelperFunc(&*F);
            //}

            //for (auto F = M.begin(), Fend = M.end(); F != Fend; ++F) {
            //  if (F->isDeclaration()) continue;
                //Spp.setScalarEvolution(&getAnalysis<ScalarEvolutionWrapperPass>(*F).getSE());
                //Spp.analyzeFunc(&*F);
            //}

            for (auto F = M.begin(), Fend = M.end(); F != Fend; ++F) {
                if (F->isDeclaration()) continue;

                Spp.visitFunc(&*F);
            }


        }
    };

    char SPPModule::ID = 0;
    static RegisterPass<SPPModule> X("spp", "Safe Persistent Pointers Pass");

}