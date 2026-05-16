#include "tree_view.hpp"
#include <FL/fl_draw.H>

const char* col_headers[COLS] = {"Name", "Size", "Percentage of parent size"};
const Fl_Align col_alignments[COLS] = {FL_ALIGN_LEFT, FL_ALIGN_RIGHT, FL_ALIGN_RIGHT}; 

void build_flat_view(FileNode* root, std::vector<ViewItem>* flat_view, unsigned short depth) {
    FileNode* curr = root;
    while (curr) {
      ViewItem item = {.node_idx = curr - file_tree_buffer, .depth = depth};
      flat_view->push_back(item);
      if (get_bit(is_expanded_mask, (uint64_t)(curr - file_tree_buffer)))
        build_flat_view(curr->first_child, flat_view, depth + 1);
      curr = curr->next_sibling;
    }
}

const char* get_item_data(int COL, uint32_t node_idx) {
  FileNode* node = file_tree_buffer + node_idx;
  switch (COL) {
    case 0:
      return node->name;
    default:
      return "0451";
  }
}

void draw_padded_text(const char *s, int X, int Y, int W, int H, Fl_Align alignment) {
  int text_x = X + CELL_TEXT_PADDING;
  int text_w = W - CELL_TEXT_PADDING*2;
  fl_color(FL_GRAY0);
  fl_draw(s, text_x,Y,text_w,H, alignment);
}

void TreeView::draw_header(const char *s, int X, int Y, int W, int H) {
    fl_push_clip(X,Y,W,H);
      fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
      fl_color(FL_BLACK);
      fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    fl_pop_clip();
}

void TreeView::draw_data(const char *s, int X, int Y, int W, int H, Fl_Align alignment) {
    fl_push_clip(X,Y,W,H);
      fl_color(FL_WHITE); 
      fl_rectf(X,Y,W,H);

      draw_padded_text(s, X, Y, W, H, alignment);

      fl_color(FL_GRAY0);
      fl_rect(X,Y,W,H);
    fl_pop_clip();
}

void TreeView::draw_progressbar(const char* s, int X, int Y, int W, int H, Fl_Align alignment, float value) {
    fl_push_clip(X,Y,W,H);
      fl_color(FL_WHITE); 
      fl_rectf(X,Y,W,H);

      fl_color(progressbar_color);
      fl_rectf(X, Y, W*value/100, H);

      draw_padded_text(s, X, Y, W, H, alignment);

      fl_color(FL_GRAY0);
      fl_rect(X,Y,W,H);
    fl_pop_clip();
}

void TreeView::draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H) {
    const char* data;
    switch ( context ) {
      case CONTEXT_STARTPAGE:
        fl_font(cell_font, cell_font_size);
        return;
      case CONTEXT_COL_HEADER:
        draw_header(col_headers[COL],X,Y,W,H);
        return;
      case CONTEXT_CELL:
        data = get_item_data(COL, flat_view[ROW].node_idx);
        if (COL == 2) {
          draw_progressbar(data, X, Y, W, H, col_alignments[COL], 42);
        } else {
          draw_data(data,X,Y,W,H, col_alignments[COL]);
        }
        return;
      default:
        return;
    }
}

void TreeView::resize(int X, int Y, int W, int H) {
    Fl_Table::resize(X, Y, W, H);

    recalc_dimensions();

    if (cols() >= 0 && tiw >= 0) {
      int width = tiw/cols();
      int remainder = tiw % cols();

      col_width_all(width);
      col_width(cols() - 1, width + remainder);
    }
}

void TreeView::set_font(Fl_Font font, int font_size) {
  cell_font = font;
  cell_font_size = font_size;
  redraw(); 
}

void TreeView::set_progressbar_color(Fl_Color color) {
  progressbar_color = color;
  redraw();
}

void TreeView::fill_flat_view(FileNode* root) {
  flat_view.clear();
  build_flat_view(root, &flat_view, 0);
  rows(flat_view.size());
  redraw();
}

TreeView::TreeView(int X, int Y, int W, int H, const char *L) : Fl_Table(X,Y,W,H,L) {
    row_height_all(ROW_HEIGHT);
    row_resize(0);

    cols(COLS);
    col_header(1);
    col_header_height(COL_HEADER_HEIGHT);
    col_resize(0);
    end();
}
