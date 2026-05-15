#pragma once

#include <vector>
#include <FL/Fl_Table.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>

#define ROW_HEIGHT 30
#define COL_HEADER_HEIGHT 30
#define COLS 3

struct ViewItem {
    uint32_t node_idx;
    uint32_t depth;
};  

class TreeView : public Fl_Table {
  Fl_Font cell_font = FL_HELVETICA;
  int cell_font_size = 16;

  std::vector<ViewItem>* data = nullptr; 

  void DrawHeader(const char *s, int X, int Y, int W, int H);

  void DrawData(const char *s, int X, int Y, int W, int H);
  
  void draw_cell(TableContext context, int ROW=0, int COL=0, int X=0, int Y=0, int W=0, int H=0) FL_OVERRIDE;

  void resize(int X, int Y, int W, int H) FL_OVERRIDE;
public:
  void set_font(Fl_Font font, int font_size);
  TreeView(int X, int Y, int W, int H, const char *L=0);
  ~TreeView() { }
};
