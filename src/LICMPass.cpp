#include "LICMPass.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

static bool isLoopInvariantInstruction(const Instruction *I, const Loop *L) {
  if (I->isTerminator() || isa<PHINode>(I) ||
      I->mayWriteToMemory() || I->mayHaveSideEffects())
    return false;
  for (const Use &U : I->operands()) {
    const Value *V = U.get();
    if (isa<Constant>(V) || isa<Argument>(V)) continue;
    if (const auto *Op = dyn_cast<Instruction>(V))
      if (L->contains(Op->getParent())) return false;
  }
  return true;
}

static SmallVector<Instruction *, 16>
collectInvariantInstructions(Loop *L) {
  SmallVector<Instruction *, 16> Worklist;
  bool Changed = true;
  while (Changed) {
    Changed = false;
    for (BasicBlock *BB : L->getBlocks())
      for (Instruction &I : *BB)
        if (isLoopInvariantInstruction(&I, L))
          if (find(Worklist, &I) == Worklist.end()) {
            Worklist.push_back(&I);
            Changed = true;
          }
  }
  return Worklist;
}

PreservedAnalyses LICMPass::run(Function &F, FunctionAnalysisManager &FAM) {
  auto &LI = FAM.getResult<LoopAnalysis>(F);
  auto &DT = FAM.getResult<DominatorTreeAnalysis>(F);
  (void)DT;
  for (Loop *L : LI.getLoopsInPreorder()) {
    if (!L->getLoopPreheader()) continue;
    auto Inv = collectInvariantInstructions(L);
    errs() << "[LICM] Found " << Inv.size() << " invariant(s) in loop depth="
           << L->getLoopDepth() << " fn='" << F.getName() << "'\n";
    for (auto *I : Inv) errs() << "       " << *I << "\n";
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
