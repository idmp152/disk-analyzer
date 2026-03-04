#pragma once
#include "clay.h"

typedef struct {
    Clay_Color primary, secondary, tertiary, highlight, background, text_primary, text_secondary;
} ColorScheme;

typedef struct {
    int font_id, font_size, window_width, window_height, padding, border_width;
} UIConfig;

static const ColorScheme g_colorscheme = { 
    .primary = { 90, 90, 90, 255 },
    .secondary = { 80, 80, 80, 255 },
    .tertiary = { 110, 110, 110, 255 },
    .highlight = { 70, 70, 70, 255 },
    .background = { 43, 41, 51, 255 },
    .text_primary = { 255, 255 , 255, 255 },
    .text_secondary = { 180, 180, 180, 255 }
};

static const UIConfig g_uiconfig = {
    .font_id = 0,
    .font_size = 20,
    .window_width = 1200,
    .window_height = 700,
    .padding = 10,
    .border_width = 2
};

#define TEXT_MAIN(color) CLAY_TEXT_CONFIG({\
                                .fontId = g_uiconfig.font_id, \
                                .fontSize = g_uiconfig.font_size, \
                                .textColor = color \
                            })

