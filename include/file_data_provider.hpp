#pragma once

#include <atomic>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "arena.hpp"

#define DEFAULT_ALIGNMENT \
	16  // TODO(IlyaBelykh): Clean up define constants, make them in a "extern
	    // const int DEFAULT_ALIGNMENT;" in header, const int DEFAULT_ALIGNMENT
	    // = 16; in impl
#define FILE_NODE_ALIGNMENT 64

#define BIT_SHIFT \
	6  // TODO(IlyaBelykh): also ambiguous name and makes it unsafe on include
#define BIT_INDEX_MASK 63

#define SCAN_PROGRESS_STEP 500

struct alignas(FILE_NODE_ALIGNMENT) FileNode {
	const char* name;
	uint64_t size;

	FileNode* parent;

	FileNode* first_child;
	FileNode* next_sibling;
};

struct DriveInfo {
	std::string name;

	uint64_t total_size;
	uint64_t free_size;

	bool is_ntfs;
};

struct ScanContext {
	const char* start_path;
	FileNode* root_node;

	std::atomic<uint64_t> files_scanned{0};

	std::atomic<bool> is_finished{false};
	std::atomic<bool> should_cancel{false};
};

inline bool get_bit(const uint64_t* mask, uint64_t idx) {
	return ((mask[idx >> BIT_SHIFT] >> (idx & BIT_INDEX_MASK)) & 1ULL) != 0U;
}

inline void set_bit(uint64_t* mask, uint64_t idx) {
	mask[idx >> BIT_SHIFT] |= (1ULL << (idx & BIT_INDEX_MASK));
}

inline void clear_bit(uint64_t* mask, uint64_t idx) {
	mask[idx >> BIT_SHIFT] &= ~(1ULL << (idx & BIT_INDEX_MASK));
}

inline void toggle_bit(uint64_t* mask, uint64_t idx) {
	mask[idx >> BIT_SHIFT] ^= (1ULL << (idx & BIT_INDEX_MASK));
}

extern uint64_t* is_directory_mask;  // TODO(IlyaBelykh): Possibly unite in a
                                     // context struct, built in main
extern uint64_t* is_expanded_mask;

extern FileNode* file_tree_buffer;
extern size_t file_tree_buffer_size;

extern Arena* string_arena;
extern std::vector<DriveInfo> drives;

void provider_init(Arena* str_arena,
                   FileNode* file_buffer,
                   uint64_t* dir_mask,
                   uint64_t* exp_mask);
char* utf16_to_utf8(const wchar_t* str);
void iterate_dir(const char* path, FileNode* parent, ScanContext* ctx);
FileNode* add_root_node(const char* path);
void traverse_tree_out(FileNode* root,
                       unsigned short depth,
                       std::ostream& stream = std::cout);
void traverse_tree_csv(FileNode* root,
                       unsigned short depth,
                       std::ostream& stream = std::cout);
void fill_drive_info();
void sort_directory_tree(FileNode* root);