#include <Fl/fl_draw.H>
#include <cmath>
#include <algorithm>
#include "treemap_widget.hpp"

FileTreeMap::FileTreeMap(int X, int Y, int W, int H, const char* L) : Fl_Widget(X, Y, W, H, L) {}

void FileTreeMap::set_root(FileNode* root) {
    root_node = root;
    redraw();
}

void draw_slice_and_dice(int x, int y, int w, int h, bool vertical, FileNode* root) {
    if (w <= 3 || h <= 3 || !root || !root->first_child) return;

    FileNode* curr = root->first_child;
    uint64_t parent_size = root->size;
    
    const int PIXEL_THRESHOLD = 6; 
    uint64_t others_combined_size = 0;
    uint64_t accumulated_size = 0;

    while (curr) {
        if (curr->size <= 0) {
            curr = curr->next_sibling;
            continue;
        }

        double fraction = (double)curr->size / parent_size;
        int projected_pixels = vertical ? std::round(h * fraction) : std::round(w * fraction);

        if (projected_pixels < PIXEL_THRESHOLD) {
            others_combined_size += curr->size;
            curr = curr->next_sibling;
            continue;
        }

        uint64_t start_size = accumulated_size;
        accumulated_size += curr->size;

        double start_fraction = (double)start_size / parent_size;
        double end_fraction = (double)accumulated_size / parent_size;

        if (vertical) {
            int start_y = y + std::round(h * start_fraction);
            int end_y = y + std::round(h * end_fraction);
            int current_h = end_y - start_y;

            if (current_h > 0) {
                fl_rect(x, start_y, w + 1, current_h + 1);
            }

            draw_slice_and_dice(x, start_y, w, current_h, !vertical, curr);
        } else {
            int start_x = x + std::round(w * start_fraction);
            int end_x = x + std::round(w * end_fraction);
            int current_w = end_x - start_x;

            if (current_w > 0) {
                fl_rect(start_x, y, current_w + 1, h + 1);
            }

            draw_slice_and_dice(start_x, y, current_w, h, !vertical, curr);
        }

        curr = curr->next_sibling;
    }

    if (others_combined_size > 0) {
        uint64_t start_size = accumulated_size;
        accumulated_size += others_combined_size;

        double start_fraction = (double)start_size / parent_size;
        double end_fraction = (double)accumulated_size / parent_size;

        if (vertical) {
            int start_y = y + std::round(h * start_fraction);
            int end_y = y + std::round(h * end_fraction);
            int current_h = end_y - start_y;

            if (current_h > 0) {
                fl_rect(x, start_y, w + 1, current_h + 1);
            }
        } else {
            int start_x = x + std::round(w * start_fraction);
            int end_x = x + std::round(w * end_fraction);
            int current_w = end_x - start_x;

            if (current_w > 0) {
                fl_rect(start_x, y, current_w + 1, h + 1);
            }
        }
    }
}

void FileTreeMap::draw() {
    fl_color(fl_rgb_color(173, 216, 230));
    fl_rectf(x(), y(), w(), h());

    if (!root_node || root_node->size == 0) return;
    
    fl_color(FL_BLACK);

    draw_slice_and_dice(x(), y(), w(), h(), false, root_node);
}