#include <math.h>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Icon.H>
#include "formats.hpp"
#include "tree_view.hpp"

const char* col_headers[COLS] = {"Name", "Size", "Percentage of parent size"};
const Fl_Align col_alignments[COLS] = {FL_ALIGN_LEFT, FL_ALIGN_RIGHT, FL_ALIGN_RIGHT};
const int col_size_ratios[COLS] = {4, 1, 2}; //  4:1:2 ratio for size, percentage and name is adequate

void build_flat_view(FileNode* root, std::vector<ViewItem>* flat_view, unsigned short depth) {
    FileNode* curr = root;
    while (curr) {
      ViewItem item = {.node_idx = (uint64_t)(curr - file_tree_buffer), .depth = depth};
      flat_view->push_back(item);
      if (get_bit(is_expanded_mask, (uint64_t)(curr - file_tree_buffer)))
        build_flat_view(curr->first_child, flat_view, depth + 1);
      curr = curr->next_sibling;
    }
}

void draw_padded_text(const char *s, int X, int Y, int W, int H, Fl_Align alignment) {
  int text_x = X + CELL_TEXT_PADDING;
  int text_w = W - CELL_TEXT_PADDING*2;
  fl_color(FL_GRAY0);
  fl_draw(s, text_x,Y,text_w,H, alignment | FL_ALIGN_INSIDE);
}

// Event handling

void TreeView::handle_events() {
    if (callback_context() != Fl_Table::CONTEXT_CELL) return;
    if (Fl::event() != FL_PUSH) return;

    int row = callback_row();
    int col = callback_col();

    if (col != NAME_COL) return;
        
    int cell_x, cell_y, cell_w, cell_h;
    find_cell(Fl_Table::CONTEXT_CELL, row, col, cell_x, cell_y, cell_w, cell_h);
    

    int depth = flat_view[row].depth*DEPTH_MULTIPLIER; 
    
    int icon_x_min = cell_x + depth;
    int icon_x_max = cell_x + depth + cell_h;
    
    if (Fl::event_x() >= icon_x_min && Fl::event_x() <= icon_x_max) {
        toggle_bit(is_expanded_mask, flat_view[row].node_idx);
        fill_flat_view(root_node);
    }
}

void TreeView::table_callback(Fl_Widget* w, void* data) {
  TreeView* tree_view = (TreeView*)w;
  tree_view->handle_events();
}

// Rendering

void TreeView::begin_draw_cell(int X, int Y, int W, int H) {
  fl_push_clip(X,Y,W,H);
  fl_color(FL_WHITE); 
  fl_rectf(X,Y,W,H);
}

void TreeView::end_draw_cell(int X, int Y, int W, int H) {
  fl_color(FL_GRAY);
  fl_rect(X,Y,W,H);
  fl_pop_clip();
}

void TreeView::draw_header(const char *s, int X, int Y, int W, int H) {
    fl_push_clip(X,Y,W,H);
      fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
      fl_color(FL_BLACK);
      fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    fl_pop_clip();
}

void TreeView::draw_name(const char* s, int X, int Y, int W, int H, int depth, bool is_directory, bool is_expanded) {
    begin_draw_cell(X, Y, W, H);
      int padding = 5;
      int icon_size = H;
      int expand_size = icon_size;

      draw_padded_text(s, X + depth + icon_size + expand_size, Y, W - depth - icon_size - expand_size, H, FL_ALIGN_LEFT);
      // fl_draw("📂", X + 5 + depth, Y + 5, icon_size, icon_size, FL_ALIGN_CENTER); TODO(IlyaBelykh): Possibly load an emoji font from the Segoe UI pack and make an icon decider function based on the extension
      fl_draw_symbol((is_directory ?"@fileopen" : "@filenew"), X + padding + depth + expand_size, Y + padding, icon_size - padding, icon_size - padding, ( is_directory ? FL_YELLOW : FL_GRAY));
      fl_color(FL_GRAY0);
      if (is_directory) {
        fl_draw(is_expanded ? "-" : "+", X + depth, Y, expand_size, expand_size, FL_ALIGN_CENTER);
        fl_rect(X + depth + padding, Y + padding + 2, expand_size - 2*padding, expand_size - 2*padding);
      }
    end_draw_cell(X, Y, W, H);
}

void TreeView::draw_data(const char *s, int X, int Y, int W, int H, Fl_Align alignment) {
    begin_draw_cell(X, Y, W, H);
      draw_padded_text(s, X, Y, W, H, alignment);
    end_draw_cell(X, Y, W, H);
}

void TreeView::draw_progressbar(const char* s, int X, int Y, int W, int H, Fl_Align alignment, float value) {
    begin_draw_cell(X, Y, W, H);

      fl_color(progressbar_color);
      fl_rectf(X, Y, W*value, H);

      draw_padded_text(s, X, Y, W, H, alignment);

    end_draw_cell(X, Y, W, H);
}

void TreeView::draw_content_cell(int ROW, int COL, int X, int Y, int W, int H) {
  uint32_t node_idx = flat_view[ROW].node_idx;
  FileNode* node = file_tree_buffer + node_idx;
  char char_buf[32];
  switch (COL) {
    case NAME_COL:
      draw_name(node->name, X, Y, W, H, flat_view[ROW].depth*DEPTH_MULTIPLIER, get_bit(is_directory_mask, node_idx), get_bit(is_expanded_mask, node_idx));
      break;
    case SIZE_COL:
      get_size_string(node->size, char_buf, 32);
      draw_data(char_buf, X, Y, W, H, col_alignments[COL]);
      break;
    case SIZE_PERCENT_COL: {
      double percentage = 1;
      if (node->parent) {
        percentage = get_size_percent_string(node->size, node->parent->size, char_buf, 32);
      } else {
        strcpy(char_buf, "100.0 %");
      }
      draw_progressbar(char_buf, X, Y, W, H, col_alignments[COL], percentage);
      break;
    }
    default:
      draw_data("0451", X, Y, W, H, col_alignments[COL]);
      break;
  }
}

// Other
void TreeView::recalculate_sizes() {
  recalc_dimensions();

  if (!(cols() >= 0 && tiw >= 0))
    return;

  int ratio_sum = 0;
  for (int i = 0; i < cols(); ++i) {
    ratio_sum += col_size_ratios[i];
  }

  int size_sum = 0;
  for (int i = 0; i < cols() - 1; ++i) {
    int curr_size = tiw*col_size_ratios[i]/ratio_sum;
    size_sum += curr_size;
    col_width(i, curr_size);
  }
  col_width(cols() - 1, tiw - size_sum);
}

// FLTK functions

void TreeView::draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H) {
    switch ( context ) {
      case CONTEXT_STARTPAGE:
        fl_font(cell_font, cell_font_size);
        return;
      case CONTEXT_COL_HEADER:
        draw_header(col_headers[COL],X,Y,W,H);
        return;
      case CONTEXT_CELL:
        draw_content_cell(ROW, COL, X, Y, W, H);
        return;
      default:
        return;
    }
}

void TreeView::resize(int X, int Y, int W, int H) {
    Fl_Table::resize(X, Y, W, H);

    recalculate_sizes();
}

// Public functions

TreeView::TreeView(int X, int Y, int W, int H, const char *L) : Fl_Table(X,Y,W,H,L) {
    callback(table_callback, nullptr);
    when(FL_WHEN_CHANGED);

    row_height_all(ROW_HEIGHT);
    row_resize(0);

    cols(COLS);
    col_header(1);
    col_header_height(COL_HEADER_HEIGHT);
    col_resize(0);
    end();
}

void TreeView::fill_flat_view(FileNode* root) {
  flat_view.clear();
  root_node = root;
  build_flat_view(root, &flat_view, 0);
  rows(flat_view.size());
  recalculate_sizes();
  redraw();
}

void TreeView::set_progressbar_color(Fl_Color color) {
  progressbar_color = color;
  redraw();
}

void TreeView::set_font(Fl_Font font, int font_size) {
  cell_font = font;
  cell_font_size = font_size;
  redraw(); 
}
