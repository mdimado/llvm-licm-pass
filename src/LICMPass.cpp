#include "LICMPass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

// Stub — real implementation coming next.
PreservedAnalyses LICMPass::run(Function &F, FunctionAnalysisManager &FAM) {
  errs() << "[LICM] Visiting function: " << F.getName() << "\n";
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
