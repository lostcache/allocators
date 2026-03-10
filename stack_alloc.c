/*
 * *stack_alloc* increments the offset to indicate the current buffer offset
 *  whilst taking into account the allocation header *stack_free* frees the
 *  memory passed to it and decrements the offset to free that memory

 * *stack_resize* first checks to see if the allocation being resized was the
 *  previously performed allocation and if so, the same pointer will be returned
 *  and the buffer offset is changed. Otherwise, stack_alloc will be called
 *  instead.

 * *stack_free_all* is used to free all the memory within the allocator
 *  by setting the buffer offsets to zero.
 */

#include <stdlib.h>

#define STACK_SIZE 1024

typedef struct AllocHeader AllocHeader;

struct AllocHeader {
  size_t prev_alloc_size;
};

typedef struct StackAlloc StackAlloc;

struct StackAlloc {
  size_t size;
  size_t header_size;
  char *buf;
};

StackAlloc *stack_init() {
  char *buffer = malloc(STACK_SIZE);
  AllocHeader *header = malloc(sizeof(AllocHeader));
  *header = (AllocHeader){0};
  StackAlloc *stack = malloc(sizeof(StackAlloc));
  *stack = (StackAlloc){STACK_SIZE, sizeof(AllocHeader), buffer};
  return stack;
}

int main() { return 0; }
