#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <vector>
#include "file_data_provider.hpp"

struct TreeMapRect {
    int x, y, w, h;
};

class FileTreeMap : public Fl_Widget {
private:
    FileNode* root_node = nullptr;

protected:
    void draw() FL_OVERRIDE;
public:
    FileTreeMap(int X, int Y, int W, int H, const char* L = nullptr);
    void set_root(FileNode* root);
};