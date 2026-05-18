#pragma once

#include <FL/Fl_Flex.H>
#include "file_data_provider.hpp"

#define MAIN_FONT 16
#define MAIN_FONT_BOLD 17
#define FONT_SIZE 16

#define TOP_ROW_HEIGHT 100
#define INFO_CONTAINER_SIZE 500 
#define TREEMAP_WIDGET_SIZE 400

#define DEFAULT_WINDOW_WIDTH 1000
#define DEFAULT_WINDOW_HEIGHT 700

#define MARGIN 10
#define TEXT_ROW_HEIGHT 25

#define ANALYZER_SECTION_ROW_GAP 30

#define PROGRESSBAR_COLOR 0x5ced7300


Fl_Flex* main_div(FileNode* root);