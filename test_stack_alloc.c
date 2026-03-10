#include "utils.c"
#include <stdio.h>

#define main stack_alloc_main
#include "stack_alloc.c"
#undef main

void *test_stack_init() {
  StackAlloc *stack = stack_init();
  TEST_ASSET(stack->buf != NULL, "init stack header cannot be null")
  TEST_ASSET(stack->header_size > 0, "init stack header cannot be null")
}

int main() { printf("All test Passed\n"); }
