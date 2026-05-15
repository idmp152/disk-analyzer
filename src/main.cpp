#include <windows.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include "arena.hpp"
#include "ui.hpp"

#define FILE_TREE_BUFFER_CAPACITY 100 * 1024 * 1024
#define STRING_BUFFER_CAPACITY 80 * 1024 * 1024
#define DEFAULT_ALIGNMENT 16

struct FileNode {
    const char* name;
    size_t size;
    
    FileNode* parent;

    FileNode* first_child;
    FileNode* next_sibling;
};

uint64_t* is_directory_mask;
uint64_t* is_expanded_mask;

FileNode* file_tree_buffer;
size_t file_tree_buffer_size = 0;

Arena string_arena;
void* string_buffer;

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

char* utf16_to_utf8(const wchar_t* str) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    char* new_str = (char*)arena_alloc_align(&string_arena, size_needed + 1, DEFAULT_ALIGNMENT); //TODO(IlyaBelykh): handle out of memory possible exceptions
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
                iterate_dir(path + L"\\" + data.cFileName, file);
            }
        } while (FindNextFileW(hFind, &data));
        FindClose(hFind);
        parent->first_child = dummy_node.next_sibling;
    }
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

void build_flat_view(FileNode* root) {
    //TODO(IlyaBelykh): Build flat view here
}

int main(int argc, char **argv) {
    SetConsoleOutputCP(CP_UTF8);

    file_tree_buffer = (FileNode*)malloc(FILE_TREE_BUFFER_CAPACITY);
    string_buffer = malloc(STRING_BUFFER_CAPACITY);
    size_t mask_bytes = (((FILE_TREE_BUFFER_CAPACITY / sizeof(FileNode)) + 63) / 64) * sizeof(uint64_t);
    is_directory_mask = (uint64_t*)calloc(1, mask_bytes);
    is_expanded_mask = (uint64_t*)calloc(1, mask_bytes);
    arena_init(&string_arena, string_buffer, STRING_BUFFER_CAPACITY);

    FileNode* root = &file_tree_buffer[file_tree_buffer_size++]; // gather info for the first scannable directory and make it the root node?
    root->name = "Soulseek Downloads";
    iterate_dir(L"C:\\Users\\overwrite\\Documents\\Soulseek Downloads", root);
    std::cout << "Iterating over the tree" << std::endl << std::endl;
    traverse_tree_cout(root, 0);

    Fl_Double_Window *window = new Fl_Double_Window(1280, 800, "Disk Analyzer");

    Fl_Flex* main_container = main_div();

    // window->resizable(main_container); // TODO(IlyaBelykh): Figure out minimal sizes for the window so that it's resized properly
    window->end();
    window->show(argc, argv);

    Fl::run();
    free(file_tree_buffer);
    free(string_buffer);
    free(is_directory_mask);
    free(is_expanded_mask);

    return 0;
}

