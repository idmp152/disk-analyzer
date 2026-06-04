#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <windows.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "arena.hpp"
#include "file_data_provider.hpp"
#include "ui.hpp"

#define FILE_TREE_BUFFER_CAPACITY (100 * 1024 * 1024)
#define STRING_BUFFER_CAPACITY (80 * 1024 * 1024)

void* string_buffer;

void window_callback(Fl_Widget* w, void* data) {
  if (Fl::callback_reason() == FL_REASON_CANCELLED)
    return;

  if (g_ui_state.is_scanning && (g_ui_state.current_ctx != nullptr)) {
    g_ui_state.current_ctx->should_cancel = true;

    if (g_ui_state.scan_thread.joinable()) {
      g_ui_state.scan_thread.join();
    }

    delete g_ui_state.current_ctx;
  }

  w->hide();
}

int WINAPI WinMain(HINSTANCE hInstance, // NOLINT(readability-inconsistent-declaration-parameter-name)
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
  SetConsoleOutputCP(CP_UTF8);
  Fl::get_system_colors();

  auto* file_tree_buffer = (FileNode*)malloc(FILE_TREE_BUFFER_CAPACITY);

  Arena string_arena;
  string_buffer = malloc(STRING_BUFFER_CAPACITY);
  arena_init(&string_arena, string_buffer, STRING_BUFFER_CAPACITY);

  size_t mask_bytes =
      (((FILE_TREE_BUFFER_CAPACITY / sizeof(FileNode)) + 63) / 64) *
      sizeof(uint64_t);
  auto* is_directory_mask = (uint64_t*)calloc(1, mask_bytes);
  auto* is_expanded_mask = (uint64_t*)calloc(1, mask_bytes);

  provider_init(&string_arena, file_tree_buffer, is_directory_mask,
                is_expanded_mask);
  fill_drive_info();

  Fl::set_font(MAIN_FONT, "Segoe UI");
  Fl::set_font(MAIN_FONT_BOLD, "BSegoe UI");

  auto* window = new Fl_Double_Window(DEFAULT_WINDOW_WIDTH,
                                      DEFAULT_WINDOW_HEIGHT, "Disk Analyzer");

  HICON hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(101));
  if (hIcon != nullptr) {
    window->icon((const void*)hIcon);
  }

  Fl_Flex* main_container = main_div();

  window->resizable(main_container);
  window->size_range(INFO_CONTAINER_SIZE * 2,
                     TOP_ROW_HEIGHT + TREEMAP_WIDGET_SIZE);
  window->end();
  window->show();
  window->callback(window_callback);

  Fl::run();

  free(file_tree_buffer);
  free(string_buffer);
  free(is_directory_mask);
  free(is_expanded_mask);

  return 0;
}
