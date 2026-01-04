# llvm-loop-optimizer

A custom LLVM optimization pass implementing **Loop Invariant Code Motion (LICM)**.

Detects loops, identifies instructions whose operands are all loop-invariant,
and hoists them to the loop preheader — reducing redundant work inside hot loops.

## What is LICM?

```c
// Before
for (int i = 0; i < n; i++) {
    int x = a * b;       // recomputed n times
    arr[i] = x + i;
}

// After
int x = a * b;           // hoisted — computed once
for (int i = 0; i < n; i++) {
    arr[i] = x + i;
}
```

## Project structure

```
llvm-loop-optimizer/
├── include/LICMPass.h    ← pass declaration
├── src/LICMPass.cpp      ← pass implementation
├── test/sample.c         ← test cases
└── CMakeLists.txt
```

## Prerequisites

| Tool | Version |
|------|---------|
| LLVM + Clang | 14 – 17 |
| CMake | >= 3.20 |
| C++ | C++17 |

**Ubuntu:** `sudo apt install llvm-17 llvm-17-dev clang-17 cmake`  
**macOS:** `brew install llvm cmake && export PATH="$(brew --prefix llvm)/bin:$PATH"`

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR=$(llvm-config --cmakedir)
cmake --build build
```

## Run

```bash
clang -O0 -S -emit-llvm test/sample.c -o build/sample.ll
opt -load-pass-plugin=./build/LICMPass.so -passes="licm-custom" \
    -S build/sample.ll -o build/sample_opt.ll
diff build/sample.ll build/sample_opt.ll
```

Expected output:
```
[LICM] Hoisting:   %mul = mul nsw i32 %a, %b
[LICM] Hoisted 1 instruction(s) from loop at depth 1 in function 'basic_licm'
```

CMake shortcut: `cmake --build build --target run-test`

## How it works

```
for each Loop (getLoopsInPreorder)
  └─ verify preheader
  └─ collectInvariantInstructions()  [fixed-point iteration]
       skip: terminators, PHIs, side-effects, memory writes
       keep: all operands outside loop
  └─ hoist each → Instruction::moveBefore(preheader terminator)
```

## Key LLVM APIs

| API | Purpose |
|-----|---------|
| `LoopAnalysis` | Enumerate loops |
| `DominatorTreeAnalysis` | Dominance info |
| `Loop::getLoopPreheader()` | Safe insertion point |
| `Instruction::moveBefore()` | Perform hoist |
| `PassBuilder` plugin API | Load via `-load-pass-plugin` |

## Test cases

| Function | Tests | Expected |
|----------|-------|----------|
| `basic_licm` | Single invariant `a*b` | Hoisted |
| `chain_licm` | Chain `p=a*b; q=p+1` | Both hoisted |
| `no_hoist` | `i*2` (loop-variant) | Not hoisted |
| `nested_licm` | Two nesting levels | Each to correct preheader |

## Skills demonstrated

LLVM IR analysis · compiler pass development · loop optimization · C++ systems programming · CMake

## License

MIT
