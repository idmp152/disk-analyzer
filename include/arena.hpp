#pragma once
#include <cstdint>

struct Arena {
  unsigned char* buf;
  size_t buf_len;
  size_t offset;
};

bool is_power_of_two(uintptr_t ptr);
uintptr_t align_forward(uintptr_t ptr, size_t align);

void* arena_alloc_align(Arena* arena, size_t size, size_t align);
void arena_init(Arena* arena, void* buffer, size_t buffer_len);
void arena_free_all(Arena* arena);