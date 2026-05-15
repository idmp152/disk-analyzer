#include "tree_view.hpp"
#include <FL/fl_draw.H>

//TODO(IlyaBelykh): Align size and percentage on the right so units of measure end up on the same side
//TODO(IlyaBelykh): For percentage column, in CONTEXT_CELL case render a progressbar instead of just text
const char* col_headers[COLS] = {"File", "Size", "Percentage of parent size"};

void TreeView::DrawHeader(const char *s, int X, int Y, int W, int H) {
    fl_push_clip(X,Y,W,H);
      fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
      fl_color(FL_BLACK);
      fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
    fl_pop_clip();
}

void TreeView::DrawData(const char *s, int X, int Y, int W, int H) { //TODO(IlyaBelykh): rename
    fl_push_clip(X,Y,W,H);
      fl_color(FL_WHITE); fl_rectf(X,Y,W,H);
      fl_color(FL_GRAY0); fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
      fl_color(FL_GRAY0); fl_rect(X,Y,W,H);
    fl_pop_clip();
}

void TreeView::draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H) {
    switch ( context ) {
      case CONTEXT_STARTPAGE:
        fl_font(cell_font, cell_font_size);
        return;
      case CONTEXT_COL_HEADER:
        DrawHeader(col_headers[COL],X,Y,W,H);
        return;
      case CONTEXT_CELL:
        DrawData("0451",X,Y,W,H);
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

TreeView::TreeView(int X, int Y, int W, int H, const char *L) : Fl_Table(X,Y,W,H,L) {
    //TODO(IlyaBelykh): Get data from actual data vector
    rows(20);
    row_height_all(ROW_HEIGHT);
    row_resize(0);

    cols(COLS);
    col_header(1);
    col_header_height(COL_HEADER_HEIGHT);
    col_resize(0);
    end();
}
