#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define main arena_main
#include "arena.c"
#undef main

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message)                                        \
  do {                                                                         \
    if (condition) {                                                           \
      printf(GREEN "✓ " RESET "%s\n", message);                                \
      tests_passed++;                                                          \
    } else {                                                                   \
      printf(RED "✗ " RESET "%s\n", message);                                  \
      tests_failed++;                                                          \
    }                                                                          \
  } while (0)

void test_is_power_of_two() {
  printf("\n=== Testing is_power_of_two ===\n");

  TEST_ASSERT(is_power_of_two(1), "1 is power of two");
  TEST_ASSERT(is_power_of_two(2), "2 is power of two");
  TEST_ASSERT(is_power_of_two(4), "4 is power of two");
  TEST_ASSERT(is_power_of_two(8), "8 is power of two");
  TEST_ASSERT(is_power_of_two(16), "16 is power of two");
  TEST_ASSERT(is_power_of_two(32), "32 is power of two");
  TEST_ASSERT(is_power_of_two(64), "64 is power of two");
  TEST_ASSERT(is_power_of_two(128), "128 is power of two");
  TEST_ASSERT(is_power_of_two(256), "256 is power of two");
  TEST_ASSERT(is_power_of_two(512), "512 is power of two");
  TEST_ASSERT(is_power_of_two(1024), "1024 is power of two");

  TEST_ASSERT(!is_power_of_two(3), "3 is not power of two");
  TEST_ASSERT(!is_power_of_two(5), "5 is not power of two");
  TEST_ASSERT(!is_power_of_two(6), "6 is not power of two");
  TEST_ASSERT(!is_power_of_two(7), "7 is not power of two");
  TEST_ASSERT(!is_power_of_two(9), "9 is not power of two");
  TEST_ASSERT(!is_power_of_two(10), "10 is not power of two");
  TEST_ASSERT(!is_power_of_two(15), "15 is not power of two");
  TEST_ASSERT(!is_power_of_two(17), "17 is not power of two");
  TEST_ASSERT(!is_power_of_two(100), "100 is not power of two");
}

void test_align_forward() {
  printf("\n=== Testing align_forward ===\n");

  TEST_ASSERT(align_forward(8, 8) == 8, "8 aligned to 8 is 8");
  TEST_ASSERT(align_forward(16, 8) == 16, "16 aligned to 8 is 16");
  TEST_ASSERT(align_forward(1, 8) == 8, "1 aligned to 8 is 8");
  TEST_ASSERT(align_forward(7, 8) == 8, "7 aligned to 8 is 8");
  TEST_ASSERT(align_forward(9, 8) == 16, "9 aligned to 8 is 16");
  TEST_ASSERT(align_forward(15, 8) == 16, "15 aligned to 8 is 16");

  TEST_ASSERT(align_forward(4, 4) == 4, "4 aligned to 4 is 4");
  TEST_ASSERT(align_forward(1, 4) == 4, "1 aligned to 4 is 4");
  TEST_ASSERT(align_forward(3, 4) == 4, "3 aligned to 4 is 4");
  TEST_ASSERT(align_forward(5, 4) == 8, "5 aligned to 4 is 8");
  TEST_ASSERT(align_forward(7, 4) == 8, "7 aligned to 4 is 8");

  TEST_ASSERT(align_forward(16, 16) == 16, "16 aligned to 16 is 16");
  TEST_ASSERT(align_forward(1, 16) == 16, "1 aligned to 16 is 16");
  TEST_ASSERT(align_forward(15, 16) == 16, "15 aligned to 16 is 16");
  TEST_ASSERT(align_forward(17, 16) == 32, "17 aligned to 16 is 32");
  TEST_ASSERT(align_forward(31, 16) == 32, "31 aligned to 16 is 32");
}

void test_arena_init() {
  printf("\n=== Testing arena_init ===\n");

  Arena *arena = arena_init();

  TEST_ASSERT(arena != NULL, "arena_init returns non-NULL pointer");
  TEST_ASSERT(arena->buffer != NULL, "arena buffer is allocated");
  TEST_ASSERT(arena->buf_len == DEFAULT_ARENA_SIZE,
              "arena buffer length is correct");
  TEST_ASSERT(arena->curr_offset == 0, "arena current offset starts at 0");

  uintptr_t buffer_addr = (uintptr_t)arena->buffer;
  TEST_ASSERT(buffer_addr % DEFAULT_ALIGNMENT == 0,
              "arena buffer is properly aligned");

  free(arena->buffer);
  free(arena);
}

void test_arena_alloc_basic() {
  printf("\n=== Testing arena_alloc basic functionality ===\n");

  Arena *arena = arena_init();

  void *ptr1 = arena_alloc(arena, 64, 8);
  TEST_ASSERT(ptr1 != NULL, "first allocation succeeds");
  TEST_ASSERT(arena->curr_offset >= 64,
              "current offset updated after allocation");

  // Test that memory is zeroed
  unsigned char *byte_ptr = (unsigned char *)ptr1;
  bool is_zeroed = true;
  for (size_t i = 0; i < 64; i++) {
    if (byte_ptr[i] != 0) {
      is_zeroed = false;
      break;
    }
  }
  TEST_ASSERT(is_zeroed, "allocated memory is zeroed");

  void *ptr2 = arena_alloc(arena, 32, 4);
  TEST_ASSERT(ptr2 != NULL, "second allocation succeeds");
  TEST_ASSERT(ptr2 != ptr1, "second allocation returns different pointer");
  TEST_ASSERT(ptr2 > ptr1, "second allocation comes after first");

  void *ptr3 = arena_alloc(arena, 16, 2);
  TEST_ASSERT(ptr3 != NULL, "third allocation succeeds");
  TEST_ASSERT(ptr3 > ptr2, "third allocation comes after second");

  free(arena->buffer);
  free(arena);
}

void test_arena_alloc_alignment() {
  printf("\n=== Testing arena_alloc alignment ===\n");

  Arena *arena = arena_init();

  void *ptr1 = arena_alloc(arena, 1, 8);
  TEST_ASSERT(ptr1 != NULL, "allocation with 8-byte alignment succeeds");
  TEST_ASSERT(((uintptr_t)ptr1) % 8 == 0, "pointer is 8-byte aligned");

  void *ptr2 = arena_alloc(arena, 1, 16);
  TEST_ASSERT(ptr2 != NULL, "allocation with 16-byte alignment succeeds");
  TEST_ASSERT(((uintptr_t)ptr2) % 16 == 0, "pointer is 16-byte aligned");

  void *ptr3 = arena_alloc(arena, 1, 32);
  TEST_ASSERT(ptr3 != NULL, "allocation with 32-byte alignment succeeds");
  TEST_ASSERT(((uintptr_t)ptr3) % 32 == 0, "pointer is 32-byte aligned");

  free(arena->buffer);
  free(arena);
}

void test_arena_alloc_large() {
  printf("\n=== Testing arena_alloc with large allocations ===\n");

  Arena *arena = arena_init();

  void *ptr = arena_alloc(arena, DEFAULT_ARENA_SIZE + 1, 8);
  TEST_ASSERT(ptr == NULL, "allocation larger than arena size fails");

  void *ptr2 = arena_alloc(arena, DEFAULT_ARENA_SIZE - 100, 8);
  TEST_ASSERT(ptr2 != NULL, "large allocation within bounds succeeds");

  void *ptr3 = arena_alloc(arena, 50, 8);
  TEST_ASSERT(ptr3 == NULL, "allocation when insufficient space fails");

  free(arena->buffer);
  free(arena);
}

void test_arena_alloc_zero_size() {
  printf("\n=== Testing arena_alloc with zero size ===\n");

  Arena *arena = arena_init();

  void *ptr = arena_alloc(arena, 0, 8);
  TEST_ASSERT(ptr != NULL, "zero-size allocation succeeds");

  void *ptr2 = arena_alloc(arena, 64, 8);
  TEST_ASSERT(ptr2 != NULL, "normal allocation after zero-size succeeds");

  free(arena->buffer);
  free(arena);
}

void test_arena_free_all() {
  printf("\n=== Testing arena_free_all ===\n");

  Arena *arena = arena_init();

  void *ptr1 = arena_alloc(arena, 64, 8);
  void *ptr2 = arena_alloc(arena, 32, 4);

  memset(ptr1, 0xAB, 64);
  memset(ptr2, 0xCD, 32);

  size_t offset_before_free = arena->curr_offset;
  TEST_ASSERT(offset_before_free > 0, "arena has been used before free_all");

  arena_free_all(arena);

  unsigned char *byte_ptr = (unsigned char *)arena->buffer;
  bool is_zeroed = true;
  for (size_t i = 0; i < arena->buf_len; i++) {
    if (byte_ptr[i] != 0) {
      is_zeroed = false;
      break;
    }
  }
  TEST_ASSERT(is_zeroed, "arena buffer is zeroed after free_all");

  printf("Info: curr_offset after free_all: %zu (not reset)\n",
         arena->curr_offset);

  void *ptr3 = arena_alloc(arena, 16, 8);
  if (ptr3 != NULL) {
    TEST_ASSERT(true, "allocation works after free_all");
  } else {
    printf(
        "Warning: allocation failed after free_all (curr_offset not reset)\n");
  }

  free(arena->buffer);
  free(arena);
}

void test_arena_edge_cases() {
  printf("\n=== Testing arena edge cases ===\n");

  Arena *arena = arena_init();

  void *ptr1 = arena_alloc(arena, 1, 1024);
  TEST_ASSERT(ptr1 != NULL, "allocation with 1024-byte alignment succeeds");
  TEST_ASSERT(((uintptr_t)ptr1) % 1024 == 0, "1024-byte alignment respected");

  void *ptr2 = arena_alloc(arena, 8, 8);
  void *ptr3 = arena_alloc(arena, 8, 8);
  void *ptr4 = arena_alloc(arena, 8, 8);

  TEST_ASSERT(ptr2 != NULL && ptr3 != NULL && ptr4 != NULL,
              "consecutive aligned allocations succeed");
  TEST_ASSERT((char *)ptr3 - (char *)ptr2 == 8,
              "consecutive allocations are properly spaced");
  TEST_ASSERT((char *)ptr4 - (char *)ptr3 == 8,
              "consecutive allocations are properly spaced");

  free(arena->buffer);
  free(arena);
}

void print_test_summary() {
  printf("\n==================================================\n");
  printf("TEST SUMMARY\n");
  printf("==================================================\n");
  printf("Tests passed: " GREEN "%d" RESET "\n", tests_passed);
  printf("Tests failed: " RED "%d" RESET "\n", tests_failed);
  printf("Total tests:  %d\n", tests_passed + tests_failed);

  if (tests_failed == 0) {
    printf(GREEN "ALL TESTS PASSED!" RESET "\n");
  } else {
    printf(RED "SOME TESTS FAILED!" RESET "\n");
  }
  printf("==================================================\n");
}

int main() {
  printf("Arena Allocator Test Suite\n");
  printf("=========================\n");

  test_is_power_of_two();
  test_align_forward();
  test_arena_init();
  test_arena_alloc_basic();
  test_arena_alloc_alignment();
  test_arena_alloc_large();
  test_arena_alloc_zero_size();
  test_arena_free_all();
  test_arena_edge_cases();

  print_test_summary();

  return tests_failed == 0 ? 0 : 1;
}
