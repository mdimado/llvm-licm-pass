//===- LICMPass.h - Loop Invariant Code Motion Pass -----------------------===//
#pragma once
#include "llvm/IR/PassManager.h"
namespace llvm { class Function; }

struct LICMPass : public llvm::PassInfoMixin<LICMPass> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM);
  static bool isRequired() { return false; }
};
