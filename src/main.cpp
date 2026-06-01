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
#include "file_data_provider.hpp"

#define FILE_TREE_BUFFER_CAPACITY 100 * 1024 * 1024
#define STRING_BUFFER_CAPACITY 80 * 1024 * 1024


void* string_buffer;

void window_callback(Fl_Widget* w, void* data) {
    if (Fl::callback_reason() == FL_REASON_CANCELLED)
        return;

    w->hide();    
}

int main(int argc, char **argv) {
    SetConsoleOutputCP(CP_UTF8);

    FileNode* file_tree_buffer = (FileNode*)malloc(FILE_TREE_BUFFER_CAPACITY);

    Arena string_arena;
    string_buffer = malloc(STRING_BUFFER_CAPACITY);
    arena_init(&string_arena, string_buffer, STRING_BUFFER_CAPACITY);

    size_t mask_bytes = (((FILE_TREE_BUFFER_CAPACITY / sizeof(FileNode)) + 63) / 64) * sizeof(uint64_t);
    uint64_t* is_directory_mask = (uint64_t*)calloc(1, mask_bytes);
    uint64_t* is_expanded_mask = (uint64_t*)calloc(1, mask_bytes);

    provider_init(&string_arena, file_tree_buffer, is_directory_mask, is_expanded_mask);

    FileNode* root = add_root_node("Downloads");
    iterate_dir(L"C:\\Users\\overwrite\\Downloads", root);
    fill_drive_info();
    // std::cout << "Iterating over the tree" << std::endl << std::endl;
    // traverse_tree_cout(root, 0);

    Fl::set_font(MAIN_FONT, "Segoe UI");
    Fl::set_font(MAIN_FONT_BOLD, "BSegoe UI");

    Fl_Double_Window *window = new Fl_Double_Window(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Disk Analyzer");

    Fl_Flex* main_container = main_div(root); //TODO(IlyaBelykh): Transfer file node tree filling to a callback on the "Analyze" button in the UI, passing the root here is just for testing

    window->resizable(main_container);
    window->size_range(INFO_CONTAINER_SIZE*2, TOP_ROW_HEIGHT + TREEMAP_WIDGET_SIZE);
    window->end();
    window->show(argc, argv);
    window->callback(window_callback);

    Fl::run();
    

    free(file_tree_buffer);
    free(string_buffer);
    free(is_directory_mask);
    free(is_expanded_mask);

    return 0;
}

