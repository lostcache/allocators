#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

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
