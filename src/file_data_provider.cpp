#include <windows.h>
#include <iostream>
#include "file_data_provider.hpp"

uint64_t* is_directory_mask;
uint64_t* is_expanded_mask;

FileNode* file_tree_buffer;
size_t file_tree_buffer_size = 0;

Arena* string_arena;

void provider_init(Arena* str_arena, FileNode* file_buffer, uint64_t* dir_mask, uint64_t* exp_mask) {
    is_directory_mask = dir_mask;
    is_expanded_mask = exp_mask;
    file_tree_buffer = file_buffer;
    string_arena = str_arena;
}

char* utf16_to_utf8(const wchar_t* str) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    char* new_str = (char*)arena_alloc_align(string_arena, size_needed + 1, DEFAULT_ALIGNMENT); //TODO(IlyaBelykh): handle out of memory possible exceptions
    WideCharToMultiByte(CP_UTF8, 0, str, -1, new_str, size_needed, NULL, NULL);
    return new_str;
}

void iterate_dir(std::wstring path, FileNode* parent) {
    WIN32_FIND_DATAW data;
    HANDLE hFind = FindFirstFileW((path + L"\\*").c_str(), &data);

    FileNode dummy_node;
    dummy_node.next_sibling = nullptr;
    FileNode* prev_node = &dummy_node;
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(data.cFileName, L".") == 0 || wcscmp(data.cFileName, L"..") == 0) {
                continue;
            }

            char* filename = utf16_to_utf8(data.cFileName);
            
            FileNode* file = &file_tree_buffer[file_tree_buffer_size++]; //TODO(IlyaBelykh): handle out of memory possible exceptions
            file->name = filename;
            file->parent = parent;
            prev_node->next_sibling = file;
            prev_node = file;

            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                set_bit(is_directory_mask, (uint64_t)(file - file_tree_buffer));
                set_bit(is_expanded_mask, (uint64_t)(file - file_tree_buffer)); //TODO(IlyaBelykh): Don't expand everything, this is only for testing
                iterate_dir(path + L"\\" + data.cFileName, file);
            }
        } while (FindNextFileW(hFind, &data));
        FindClose(hFind);
        parent->first_child = dummy_node.next_sibling;
    }
}

FileNode* add_root_node(const char* path) {
    FileNode* root = &file_tree_buffer[file_tree_buffer_size++];
    set_bit(is_directory_mask, (uint64_t)(root - file_tree_buffer));
    set_bit(is_expanded_mask, (uint64_t)(root - file_tree_buffer));
    root->name = path;
    return root;
}

void traverse_tree_cout(FileNode* root, unsigned short depth) {
    FileNode* curr = root;
    while (curr) {
        std::string padding(depth*2, ' ');
        std::cout << padding << (get_bit(is_directory_mask, (uint64_t)(curr - file_tree_buffer)) ? "- " : "> ") << curr->name << std::endl;
        traverse_tree_cout(curr->first_child, depth + 1);
        curr = curr->next_sibling;
    }
}