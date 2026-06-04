#include "arena.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>

bool is_power_of_two(uintptr_t ptr) {
	return (ptr & (ptr - 1)) == 0;
}

uintptr_t align_forward(uintptr_t ptr, size_t align) {
	uintptr_t mod;
	uintptr_t new_ptr;
	uintptr_t al;

	assert(is_power_of_two(align));

	al = (uintptr_t)align;
	new_ptr = ptr;

	mod = new_ptr & (al - 1);
	if (mod != 0) {
		new_ptr += al - mod;
	}
	return new_ptr;
}

void* arena_alloc_align(Arena* arena, size_t size, size_t align) {
	uintptr_t curr_ptr = (uintptr_t)arena->buf + (uintptr_t)arena->offset;
	uintptr_t offset = align_forward(curr_ptr, align);
	offset -= (uintptr_t)arena->buf;

	if (offset + size <= arena->buf_len) {
		void* ptr = &arena->buf[offset];
		arena->offset = offset + size;

		return ptr;
	}
	return nullptr;
}

void arena_init(Arena* arena, void* buffer, size_t buffer_len) {
	arena->buf = (unsigned char*)buffer;
	arena->buf_len = buffer_len;
	arena->offset = 0;
}

void arena_free_all(Arena* arena) {
	arena->offset = 0;
}