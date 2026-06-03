#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>
#include <vector>
#include "file_data_provider.hpp"

#define ROW_HEIGHT 30
#define COL_HEADER_HEIGHT 30
#define COLS 3
#define CELL_TEXT_PADDING 10

#define DEPTH_MULTIPLIER 15

enum TreeViewColumns { NAME_COL, SIZE_COL, SIZE_PERCENT_COL };

struct ViewItem {
  uint64_t node_idx;
  uint64_t depth;
};

class TreeView : public Fl_Table {
 public:
  TreeView(int X, int Y, int W, int H, const char* L = 0);

  void fill_flat_view(FileNode* root);
  void set_progressbar_color(Fl_Color color);
  void set_font(Fl_Font font, int font_size);

 protected:
  // FLTK methods
  void draw_cell(TableContext context,
                 int ROW = 0,
                 int COL = 0,
                 int X = 0,
                 int Y = 0,
                 int W = 0,
                 int H = 0) FL_OVERRIDE;
  void resize(int X, int Y, int W, int H) FL_OVERRIDE;

 private:
  // Configs
  Fl_Font cell_font = FL_HELVETICA;
  int cell_font_size = 16;
  Fl_Color progressbar_color = fl_rgb_color(197, 202, 233);

  // Data
  std::vector<ViewItem> flat_view;
  FileNode* root_node;

  // Event handling
  static void table_callback(Fl_Widget* w, void* data);
  void handle_events();

  // Rendering
  void draw_content_cell(int ROW, int COL, int X, int Y, int W, int H);
  static void begin_draw_cell(
      int X,
      int Y,
      int W,
      int H);  // TODO(IlyaBelykh): clang-tidy offered making them static
  static void end_draw_cell(
      int X,
      int Y,
      int W,
      int H);  // My counter-offer would be making them a non-member function,
               // will take a look into this later
  void draw_data(const char* s, int X, int Y, int W, int H, Fl_Align alignment);
  void draw_header(const char* s, int X, int Y, int W, int H);
  void draw_name(const char* s,
                 int X,
                 int Y,
                 int W,
                 int H,
                 int depth,
                 bool is_directory,
                 bool is_expanded);
  void draw_progressbar(const char* s,
                        int X,
                        int Y,
                        int W,
                        int H,
                        Fl_Align alignment,
                        float value);

  // Other
  void recalculate_sizes();
};