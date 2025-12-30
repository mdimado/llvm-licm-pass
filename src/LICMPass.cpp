#include "LICMPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

PreservedAnalyses LICMPass::run(Function &F, FunctionAnalysisManager &FAM) {
  auto &LI = FAM.getResult<LoopAnalysis>(F);
  auto &DT = FAM.getResult<DominatorTreeAnalysis>(F);
  (void)DT;

  for (Loop *L : LI.getLoopsInPreorder()) {
    BasicBlock *Preheader = L->getLoopPreheader();
    if (!Preheader) {
      errs() << "[LICM] Loop at depth " << L->getLoopDepth() << " has no preheader — skipping.\n";
      continue;
    }
    errs() << "[LICM] Found loop depth=" << L->getLoopDepth()
           << " fn='" << F.getName() << "' preheader=" << Preheader->getName() << "\n";
  }
  return PreservedAnalyses::all();
}

llvm::PassPluginLibraryInfo getLICMPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LICMPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "licm-custom") { FPM.addPass(LICMPass()); return true; }
                  return false;
                });
          }};
}
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() { return getLICMPassPluginInfo(); }
