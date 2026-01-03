// test/sample.c — Test input for the custom LICM pass.
//
// Build & run:
//   clang -O0 -S -emit-llvm test/sample.c -o build/sample.ll
//   opt -load-pass-plugin=./build/LICMPass.so \
//       -passes="licm-custom" -S build/sample.ll -o build/sample_opt.ll
//   diff build/sample.ll build/sample_opt.ll

#include <stdio.h>

// Test 1: basic LICM — a*b is loop-invariant, should be hoisted.
void basic_licm(int *arr, int n, int a, int b) {
    for (int i = 0; i < n; i++) {
        int x = a * b;      // hoisted
        arr[i] = x + i;
    }
}

// Test 2: invariant def-use chain — p and q both hoisted.
void chain_licm(int *arr, int n, int a, int b) {
    for (int i = 0; i < n; i++) {
        int p = a * b;      // hoisted
        int q = p + 1;      // hoisted (depends only on p)
        arr[i] = q + i;
    }
}

// Test 3: negative case — i*2 depends on loop var, must NOT be hoisted.
void no_hoist(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = i * 2;
    }
}

// Test 4: nested loops — each level has its own invariant.
void nested_licm(int *arr, int n, int a, int b, int c) {
    for (int i = 0; i < n; i++) {
        int outer = c + 1;
        for (int j = 0; j < n; j++) {
            int inner = a * b;
            arr[i * n + j] = inner + outer + i + j;
        }
    }
}

int main(void) {
    int arr[16] = {0};
    basic_licm(arr, 4, 3, 5);
    printf("basic_licm:  arr[0]=%d arr[3]=%d\n", arr[0], arr[3]);
    chain_licm(arr, 4, 3, 5);
    printf("chain_licm:  arr[0]=%d arr[3]=%d\n", arr[0], arr[3]);
    no_hoist(arr, 4);
    printf("no_hoist:    arr[0]=%d arr[3]=%d\n", arr[0], arr[3]);
    nested_licm(arr, 4, 2, 3, 10);
    printf("nested_licm: arr[0]=%d arr[15]=%d\n", arr[0], arr[15]);
    return 0;
}
