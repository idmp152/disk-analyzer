#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <vector>
#include "file_data_provider.hpp"

struct VisualRect {
  int x, y, w, h;
  FileNode* node;
};

class FileTreeMap : public Fl_Widget {
 private:
  Fl_Color main_color = fl_rgb_color(173, 216, 230);
  Fl_Color small_file_color = fl_rgb_color(140, 140, 140);

  FileNode* root_node = nullptr;
  std::vector<VisualRect> visual_elements;
  FileNode* hovered_node = nullptr;
  bool is_hovered_others = false;

  void draw_slice_and_dice(int x,
                           int y,
                           int w,
                           int h,
                           bool vertical,
                           FileNode* root);

 protected:
  void draw() FL_OVERRIDE;
  int handle(int event) FL_OVERRIDE;

 public:
  FileTreeMap(int X, int Y, int W, int H, const char* L = nullptr);
  void set_root(FileNode* root);
};