#include "ui_components.h"
#include "config.h"


static const int TABLE_ROW_HEIGHT = 40;
static const int HEADER_BAR_HEIGHT = 100;
static const int DISK_CHOICE_HEADER_WIDTH = 300;
static const int DISK_CHOICE_MENU_WIDTH = 200;
static const int PROGRAM_INFO_HEADER_WIDTH = 300;
static const int DIAGRAM_MENU_HEIGHT = 250;
static const int FIELD_FILENAME_WIDTH = 300;
static const int FIELD_SIZE_WIDTH = 150;
static const int FIELD_CREATED_AT_WIDTH = 200;
static const int FIELD_MODIFIED_AT_WIDTH = 200;

void UI_TableColumn(TableColumnConfig config) {
    CLAY({
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(config.width),
                .height = CLAY_SIZING_GROW()
            },
            .padding = CLAY_PADDING_ALL(g_uiconfig.padding)
        }
    }) {
        CLAY_TEXT(config.text, TEXT_MAIN(config.textColor));
    }
}

void UI_TableRow(TableRowConfig config) {
    CLAY({
        .backgroundColor = Clay_Hovered() ? config.hoverColor : config.inactiveColor,
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_FIXED(config.rowHeight)
            }
        }
    }) {
        UI_TableColumn((TableColumnConfig){ .text = config.file->name, .textColor = config.textColor, .width = config.filenameWidth });
        UI_TableColumn((TableColumnConfig){ .text = config.file->size, .textColor = config.textColor, .width = config.sizeWidth });
        UI_TableColumn((TableColumnConfig){ .text = config.file->created_at, .textColor = config.textColor, .width = config.createdWidth });
        UI_TableColumn((TableColumnConfig){ .text = config.file->modified_at, .textColor = config.textColor, .width = config.modifiedWidth });
    }
}

void UI_FileTableHeader(FileTableConfig config) {
    CLAY({
        .id = CLAY_ID("FileTreeHeader"),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_FIXED(config.rowHeight)
            }
        },
        .backgroundColor = config.headerBg,
        .border = { 
            .width = { .betweenChildren = g_uiconfig.border_width }, 
            .color = g_colorscheme.primary 
        }
    }) {
        UI_TableColumn((TableColumnConfig){ CLAY_STRING("Filename"), config.headerText, config.filenameWidth });
        UI_TableColumn((TableColumnConfig){ CLAY_STRING("Size"), config.headerText, config.sizeWidth });
        UI_TableColumn((TableColumnConfig){ CLAY_STRING("Created At"), config.headerText, config.createdWidth });
        UI_TableColumn((TableColumnConfig){ CLAY_STRING("Modified At"), config.headerText, config.modifiedWidth });
    }
}

void UI_FileTableContent(FileTableConfig config) {
    CLAY({
        .id = CLAY_ID("FileTreeContent"),
        .layout = {
            .sizing = CLAY_SIZING_GROW(),
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        },
        .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
    }) {
        for (int i = 0; i < config.fileCount; ++i) {
            UI_TableRow((TableRowConfig){
                .file = &config.files[i],
                .textColor = config.rowText,
                .hoverColor = config.rowHover,
                .inactiveColor = config.rowInactive,
                .filenameWidth = config.filenameWidth,
                .sizeWidth = config.sizeWidth,
                .createdWidth = config.createdWidth,
                .modifiedWidth = config.modifiedWidth,
                .rowHeight = config.rowHeight
            });
        }
    }
}

void UI_FileTable(FileTableConfig config) {
    CLAY({
        .id = CLAY_ID("FileTree"),
        .layout = {
            .sizing = CLAY_SIZING_GROW(),
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        }
    }) {
        UI_FileTableHeader(config);
        UI_FileTableContent(config);
    }
}

// ─────────────────────────────────────────────────────────────
// Header Bar Sub-Components
// ─────────────────────────────────────────────────────────────

static void HandleDropdownOptionInteraction(Clay_ElementId elementId, 
                                            Clay_PointerData pointerData, 
                                            intptr_t userData) {
    DropdownOptionData* data = (DropdownOptionData*)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        *(data->selectedIndexPtr) = data->buttonIndex;
    }
}

static void UI_DropdownOption(Clay_String text, Clay_Color textColor, 
                               Clay_Color hoverColor, Clay_Color inactiveColor,
                               DropdownOptionData* data) {
    CLAY({
        .backgroundColor = Clay_Hovered() ? hoverColor : inactiveColor,
        .layout = {
            .padding = CLAY_PADDING_ALL(g_uiconfig.padding),
            .sizing = { .width = CLAY_SIZING_GROW() }
        }
    }) {
        Clay_OnHover(HandleDropdownOptionInteraction, (intptr_t)data);
        CLAY_TEXT(text, TEXT_MAIN(textColor));
    }
}

static void UI_DropdownMenu(Clay_String* options, int count, int* selectedIndex, DropdownOptionPool* optionPool,
                            Clay_Color bgColor, Clay_Color hoverColor, 
                            Clay_Color textColor, Clay_Color borderColor,
                            int menuWidth) {
    CLAY({
        .id = CLAY_ID("DriveMenu"),
        .floating = { 
            .attachTo = CLAY_ATTACH_TO_PARENT,
            .attachPoints = {
                .element = CLAY_ATTACH_POINT_LEFT_TOP,
                .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
            }
        },
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIXED(menuWidth) }
        },
        .backgroundColor = bgColor,
        .border = { 
            .width = { .left = g_uiconfig.border_width, .right = g_uiconfig.border_width, .bottom = g_uiconfig.border_width },
            .color = borderColor 
        }
    }) {
        for (int i = 0; i < count; ++i) {
            optionPool->options[i] = (DropdownOptionData) { .buttonIndex = i, .selectedIndexPtr = selectedIndex };
            UI_DropdownOption(options[i], textColor, hoverColor, bgColor, optionPool->options + i);
        }
        optionPool->count = count;
    }
}

void UI_DiskSelector(DiskSelectorConfig config) {
    CLAY({
        .id = CLAY_ID("DiskChoiceHeader"),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(config.headerWidth),
                .height = CLAY_SIZING_GROW()
            },
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .padding = CLAY_PADDING_ALL(config.padding)
        }
    }) {
        CLAY({
            .id = CLAY_ID("DiskChoiceLine"),
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW() },
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 8
            }
        }) {
            CLAY({
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIT(),
                        .height = CLAY_SIZING_GROW()
                    }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Choose:"), TEXT_MAIN(config.textColor));
            }
            
            // Selector button + dropdown
            CLAY({
                .id = CLAY_ID("ChoiceSelector"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(),
                        .height = CLAY_SIZING_GROW()
                    },
                    .padding = { .left = 8 }
                },
                .backgroundColor = config.bgColor
            }) { 
                CLAY_TEXT(config.drives[*config.selectedIndex], TEXT_MAIN(config.textColor));
                
                if (config.isOpen) {
                    UI_DropdownMenu(config.drives, config.driveCount, config.selectedIndex, config.options,
                                   config.bgColor, config.hoverColor, config.textColor, 
                                   config.borderColor, config.menuWidth);
                }
            }
        }
    }
}

void UI_InfoLabel(InfoLabelConfig config) {
    CLAY({
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_GROW()
            },
            .padding = CLAY_PADDING_ALL(config.padding)
        }
    }) {
        CLAY_TEXT(config.label, TEXT_MAIN(config.textColor));
    }
}

void UI_ProgramTitle(PanelConfig config) {
    CLAY({
        .id = CLAY_ID("ProgramInfoHeader"),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(PROGRAM_INFO_HEADER_WIDTH), // from constants
                .height = CLAY_SIZING_GROW()
            }
        }
    }) {
        CLAY({
            .layout = {
                .sizing = CLAY_SIZING_GROW(),
                .padding = CLAY_PADDING_ALL(config.padding)
            }
        }) {
            CLAY_TEXT(config.title, TEXT_MAIN(config.textColor));
        }
    }
}

// ─────────────────────────────────────────────────────────────
// Composite Components
// ─────────────────────────────────────────────────────────────

void UI_HeaderBar(AppState* app_state) {
    CLAY({
        .id = CLAY_ID("HeaderBar"),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_FIXED(HEADER_BAR_HEIGHT)
            },
            .layoutDirection = CLAY_LEFT_TO_RIGHT
        },
        .backgroundColor = g_colorscheme.primary,
        .border = { 
            .width = { .betweenChildren = g_uiconfig.border_width }, 
            .color = g_colorscheme.secondary 
        }
    }) {
        // Disk selector dropdown
        UI_DiskSelector((DiskSelectorConfig){
            .drives = app_state->availableDrives,
            .driveCount = app_state->driveCount,
            .selectedIndex = &app_state->selectedDrive,
            .options = &app_state->dropdownOptions,
            .isOpen = app_state->dropdownOpen,
            .bgColor = g_colorscheme.tertiary,
            .hoverColor = g_colorscheme.highlight,
            .textColor = g_colorscheme.text_primary,
            .borderColor = g_colorscheme.text_primary,
            .headerWidth = DISK_CHOICE_HEADER_WIDTH,
            .menuWidth = DISK_CHOICE_MENU_WIDTH,
            .padding = g_uiconfig.padding
        });
        
        // "Selected: Drive X:" info
        CLAY({
            .id = CLAY_ID("DiskInfoHeader"),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_GROW()
                }
            }
        }) {
            CLAY({
                .id = CLAY_ID("SelectedDiskRow"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW() },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .childGap = 8
                }
            }) {
                UI_InfoLabel((InfoLabelConfig){ 
                    .label = CLAY_STRING("Selected:"), 
                    .textColor = g_colorscheme.text_primary,
                    .padding = g_uiconfig.padding 
                });
                UI_InfoLabel((InfoLabelConfig){ 
                    .label = app_state->availableDrives[app_state->selectedDrive], 
                    .textColor = g_colorscheme.text_primary,
                    .padding = g_uiconfig.padding 
                });
            }
        }
        
        // Program title
        UI_ProgramTitle((PanelConfig){
            .title = CLAY_STRING("Disk Analyzer"),
            .bgColor = g_colorscheme.primary,
            .textColor = g_colorscheme.text_primary,
            .height = HEADER_BAR_HEIGHT,
            .padding = g_uiconfig.padding
        });
    }
}

void UI_DiagramPanel(PanelConfig config) {
    CLAY({
        .id = CLAY_ID("Diagram"),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_FIXED(config.height)
            }
        },
        .backgroundColor = config.bgColor
    }) {
        // Placeholder for future diagram rendering
        // Could add CLAY_TEXT or custom drawing here
    }
}

void UI_OuterContainer(AppState* app_state) {
    CLAY({
        .id = CLAY_ID("OuterContainer"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = CLAY_SIZING_GROW(),
        },
        .backgroundColor = g_colorscheme.background,
    }) {
        UI_HeaderBar(app_state);
        
        UI_FileTable((FileTableConfig){
            .files = app_state->currentFiles,
            .fileCount = app_state->fileCount,
            .headerBg = g_colorscheme.secondary,
            .headerText = g_colorscheme.text_primary,
            .rowText = g_colorscheme.text_primary,
            .rowHover = g_colorscheme.highlight,
            .rowInactive = g_colorscheme.background,
            .rowHeight = TABLE_ROW_HEIGHT,
            .filenameWidth = FIELD_FILENAME_WIDTH,
            .sizeWidth = FIELD_SIZE_WIDTH,
            .createdWidth = FIELD_CREATED_AT_WIDTH,
            .modifiedWidth = FIELD_MODIFIED_AT_WIDTH
        });
        
        UI_DiagramPanel((PanelConfig){
            .title = CLAY_STRING(""), // unused, kept for API consistency
            .bgColor = g_colorscheme.primary,
            .textColor = g_colorscheme.text_primary,
            .height = DIAGRAM_MENU_HEIGHT,
            .padding = g_uiconfig.padding
        });
    }
}

void UI_RenderUI(AppState* app_state) {
    Clay_Sizing layoutExpand = {
        .width = CLAY_SIZING_GROW(),
        .height = CLAY_SIZING_GROW()
    };

    // Outer container now delegates everything
    CLAY({
        .layout = { .sizing = layoutExpand }
    }) {
        UI_OuterContainer(app_state);
    }
}