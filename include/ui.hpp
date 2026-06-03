#pragma once

#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Progress.H>
#include <thread>
#include "file_data_provider.hpp"
#include "tree_view.hpp"
#include "treemap_widget.hpp"

#define MAIN_FONT 16
#define MAIN_FONT_BOLD 17
#define FONT_SIZE 16

#define TOP_ROW_HEIGHT 100
#define INFO_CONTAINER_SIZE 500
#define TREEMAP_WIDGET_SIZE 400
#define MENU_BAR_SIZE 25

#define DEFAULT_WINDOW_WIDTH 1000
#define DEFAULT_WINDOW_HEIGHT 700

#define MARGIN 10
#define TEXT_ROW_HEIGHT 25

#define ANALYZER_SECTION_ROW_GAP 30

#define PROGRESSBAR_COLOR 0x5ced7300

struct UIState {
  Fl_Choice* drive_choice = nullptr;
  Fl_Box* val_selected = nullptr;
  Fl_Box* val_total = nullptr;
  Fl_Box* val_used = nullptr;
  Fl_Box* val_free = nullptr;
  Fl_Progress* progress_bar = nullptr;
  FileTreeMap* tree_map = nullptr;
  TreeView* tree_view = nullptr;

  std::thread scan_thread;
  ScanContext* current_ctx = nullptr;
  bool is_scanning = false;
  FileNode* root = nullptr;
};

extern UIState g_ui_state;
Fl_Flex* main_div();