#include "treemap_widget.hpp"
#include <FL/Fl_Tooltip.H>
#include <Fl/fl_draw.H>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "formats.hpp"
#include "tree_view.hpp"

FileTreeMap::FileTreeMap(int X, int Y, int W, int H, const char* L)
    : Fl_Widget(X, Y, W, H, L) {}

void FileTreeMap::set_root(FileNode* root) {
	root_node = root;
	redraw();
}

void FileTreeMap::draw_slice_and_dice(int x,
                                      int y,
                                      int w,
                                      int h,
                                      bool vertical,
                                      FileNode* root) {
	if (w <= 3 || h <= 3 || (root == nullptr) || (root->first_child == nullptr))
		return;

	FileNode* curr = root->first_child;
	uint64_t parent_size = root->size;

	const int PIXEL_THRESHOLD = 5;
	uint64_t others_combined_size = 0;
	uint64_t accumulated_size = 0;

	while (curr != nullptr) {
		if (curr->size <= 0) {
			curr = curr->next_sibling;
			continue;
		}

		double fraction = (double)curr->size / (double)parent_size;
		int projected_pixels = vertical ? (int)std::round(h * fraction)
		                                : (int)std::round(w * fraction);

		if (projected_pixels < PIXEL_THRESHOLD) {
			others_combined_size += curr->size;
			curr = curr->next_sibling;
			continue;
		}

		uint64_t start_size = accumulated_size;
		accumulated_size += curr->size;

		double start_fraction = (double)start_size / (double)parent_size;
		double end_fraction = (double)accumulated_size / (double)parent_size;

		if (vertical) {
			int start_y = y + (int)std::round(h * start_fraction);
			int end_y = y + (int)std::round(h * end_fraction);
			int current_h = end_y - start_y;

			if (current_h > 0) {
				fl_rect(x, start_y, w + 1, current_h + 1);

				if (curr->first_child == nullptr) {
					visual_elements.push_back({x, start_y, w, current_h, curr});
				}
			}

			draw_slice_and_dice(x, start_y, w, current_h, !vertical, curr);
		} else {
			int start_x = x + (int)std::round(w * start_fraction);
			int end_x = x + (int)std::round(w * end_fraction);
			int current_w = end_x - start_x;

			if (current_w > 0) {
				fl_rect(start_x, y, current_w + 1, h + 1);

				if (curr->first_child == nullptr) {
					visual_elements.push_back({start_x, y, current_w, h, curr});
				}
			}

			draw_slice_and_dice(start_x, y, current_w, h, !vertical, curr);
		}

		curr = curr->next_sibling;
	}

	if (others_combined_size > 0) {
		uint64_t start_size = accumulated_size;
		accumulated_size += others_combined_size;

		double start_fraction = (double)start_size / (double)parent_size;
		double end_fraction = (double)accumulated_size / (double)parent_size;

		if (vertical) {
			int start_y = y + (int)std::round(h * start_fraction);
			int end_y = y + (int)std::round(h * end_fraction);
			int current_h = end_y - start_y;

			if (current_h > 0) {
				fl_color(small_file_color);
				fl_rectf(x, start_y, w + 1, current_h + 1);

				fl_color(FL_BLACK);
				fl_rect(x, start_y, w + 1, current_h + 1);

				visual_elements.push_back({x, start_y, w, current_h, nullptr});
			}
		} else {
			int start_x = x + (int)std::round(w * start_fraction);
			int end_x = x + (int)std::round(w * end_fraction);
			int current_w = end_x - start_x;

			if (current_w > 0) {
				fl_color(small_file_color);
				fl_rectf(start_x, y, current_w + 1, h + 1);

				fl_color(FL_BLACK);
				fl_rect(start_x, y, current_w + 1, h + 1);

				visual_elements.push_back({start_x, y, current_w, h, nullptr});
			}
		}
	}
}

int FileTreeMap::handle(int event) {
	if (event == FL_LEAVE) {
		hovered_node = nullptr;
		is_hovered_others = false;
		tooltip(nullptr);
		Fl_Tooltip::enter(nullptr);
		return 1;
	}

	if (event != FL_MOVE && event != FL_ENTER) {
		return Fl_Widget::handle(event);
	}

	int mx = Fl::event_x();
	int my = Fl::event_y();

	FileNode* found_node = nullptr;
	bool found_others = false;
	bool inside_any_element = false;

	for (const auto& el : visual_elements) {
		if (mx >= el.x && mx < el.x + el.w && my >= el.y && my < el.y + el.h) {
			inside_any_element = true;
			if (el.node == nullptr) {
				found_others = true;
			} else {
				found_node = el.node;
			}
			break;
		}
	}

	if (found_node != hovered_node || found_others != is_hovered_others) {
		hovered_node = found_node;
		is_hovered_others = found_others;

		if (inside_any_element) {
			static char tooltip_buf[256];

			if (is_hovered_others) {
				snprintf(tooltip_buf, sizeof(tooltip_buf), "Small files");
			} else if ((hovered_node != nullptr) &&
			           (hovered_node->name != nullptr)) {
				char size_buf[32];
				char name_buf[128];
				get_size_string(hovered_node->size, size_buf, 32);
				get_full_path(hovered_node, name_buf, 128);
				snprintf(tooltip_buf, sizeof(tooltip_buf), "%s (%s)", name_buf,
				         size_buf);
			}

			tooltip(tooltip_buf);

			Fl_Tooltip::enter(nullptr);
			Fl_Tooltip::enter(this);
		} else {
			tooltip(nullptr);
			Fl_Tooltip::enter(nullptr);
		}
	}
	return 1;
}

void FileTreeMap::draw() {
	fl_color(main_color);
	fl_rectf(x(), y(), w(), h());

	visual_elements.clear();
	hovered_node = nullptr;
	is_hovered_others = false;

	if ((root_node == nullptr) || root_node->size == 0)
		return;

	fl_color(FL_BLACK);
	fl_line_style(FL_SOLID, 1);

	draw_slice_and_dice(x(), y(), w(), h(), false, root_node);
}