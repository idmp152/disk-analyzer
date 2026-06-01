#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "arena.hpp"

#define DEFAULT_ALIGNMENT 16

struct alignas(64) FileNode {
    const char* name;
    uint64_t size;
    
    FileNode* parent;

    FileNode* first_child;
    FileNode* next_sibling;
};

struct DriveInfo {
    const char* name;

    uint64_t total_size;
    uint64_t free_size;

    bool is_ntfs;
};

inline bool get_bit(uint64_t* mask, uint64_t idx) {
    return (mask[idx >> 6] >> (idx & 63)) & 1ULL;
}

inline void set_bit(uint64_t* mask, uint64_t idx) {
    mask[idx >> 6] |= (1ULL << (idx & 63));
}

inline void clear_bit(uint64_t* mask, uint64_t idx) {
    mask[idx >> 6] &= ~(1ULL << (idx & 63));
}

inline void toggle_bit(uint64_t* mask, uint64_t idx) {
    mask[idx >> 6] ^= (1ULL << (idx & 63));
}

extern uint64_t* is_directory_mask; //TODO(IlyaBelykh): Possibly unite in a context struct, built in main
extern uint64_t* is_expanded_mask;

extern FileNode* file_tree_buffer;
extern size_t file_tree_buffer_size;

extern Arena* string_arena;
extern std::vector<DriveInfo> drives;

void provider_init(Arena* string_arena, FileNode* file_tree_buffer, uint64_t* is_directory_mask, uint64_t* is_expanded_mask);
char* utf16_to_utf8(const wchar_t* str);
void iterate_dir(std::wstring path, FileNode* parent);
FileNode* add_root_node(const char* path);
void traverse_tree_cout(FileNode* root, unsigned short depth);
void fill_drive_info();
void sort_directory_tree(FileNode* root);