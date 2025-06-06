#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEFAULT_ALIGNMENT
// x2 for platforms that support bigger registers or SIMD
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#define DEFAULT_ARENA_SIZE 16 * 100
#endif

bool is_power_of_two(uintptr_t ptr) {
  assert(ptr > 0);

  return (ptr & (ptr - 1)) == 0;
}

typedef struct Arena Arena;

struct Arena {
  unsigned char *buffer;
  size_t buf_len;
  size_t curr_offset;
};

uintptr_t align_forward(uintptr_t ptr, uintptr_t alignment) {
  assert(is_power_of_two(alignment));

  uintptr_t mod = ptr & (alignment - 1);

  if (mod != 0) {
    ptr += alignment - mod;
  }

  return ptr;
}

void *arena_alloc_align(Arena *arena, size_t size, size_t align) {
  uintptr_t end_ptr = (uintptr_t)arena->buffer + (uintptr_t)arena->curr_offset;
  end_ptr = align_forward(end_ptr, align);
  uintptr_t filled_space = end_ptr - (uintptr_t)arena->buffer;

  if (filled_space > arena->buf_len - size) {
    return NULL;
  }

  arena->curr_offset = filled_space + size;
  void *ptr = (void *)&arena->buffer[filled_space];
  memset(ptr, 0, size);
  return ptr;
}

Arena *arena_init() {
  void *buffer = aligned_alloc(DEFAULT_ALIGNMENT, DEFAULT_ARENA_SIZE);
  Arena *arena = malloc(sizeof(Arena));
  *arena = (Arena){buffer, DEFAULT_ARENA_SIZE, 0};
  return arena;
}

void *arena_alloc(Arena *arena, size_t size, size_t align) {
  return arena_alloc_align(arena, size, align);
}

void arena_free_all(Arena *arena) { memset(arena->buffer, 0, arena->buf_len); }

int main() { Arena *arena = arena_init(); }
