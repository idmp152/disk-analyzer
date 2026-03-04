#include "ui_components.h"
#include "config.h"
#include <stdio.h>


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
static const int TREE_INDENT = 12;
static const int EXPAND_ICON_WIDTH = 28;

static uint32_t StringHash(const char* str) {
    if (!str) return 0;
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

static void HandleDropdownOptionInteraction(Clay_ElementId elementId, 
                                            Clay_PointerData pointerData, 
                                            void* userData) {
    DropdownOptionData* data = (DropdownOptionData*)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        *(data->selectedIndexPtr) = data->buttonIndex;
    }
}

static void UI_DropdownOption(Clay_String text, Clay_Color textColor, 
                               Clay_Color hoverColor, Clay_Color inactiveColor,
                               DropdownOptionData* data) {
    CLAY(CLAY_IDI("DropdownOption", data->buttonIndex), {
        .backgroundColor = Clay_Hovered() ? hoverColor : inactiveColor,
        .layout = {
            .padding = CLAY_PADDING_ALL(g_uiconfig.padding),
            .sizing = { .width = CLAY_SIZING_GROW() }
        }
    }) {
        Clay_OnHover(HandleDropdownOptionInteraction, (void*)data);
        CLAY_TEXT(text, TEXT_MAIN(textColor));
    }
}

static void UI_DropdownMenu(Clay_String* options, int count, int* selectedIndex, DropdownOptionPool* optionPool,
                            Clay_Color bgColor, Clay_Color hoverColor, 
                            Clay_Color textColor, Clay_Color borderColor,
                            int menuWidth) {
    CLAY(CLAY_ID("DriveMenu"), {
        .floating = { 
            .attachTo = CLAY_ATTACH_TO_PARENT,
            .attachPoints = {
                .element = CLAY_ATTACH_POINT_LEFT_TOP,
                .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
            },
            .zIndex = 100
        },
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIXED(menuWidth), .height = CLAY_SIZING_FIT() }
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
    CLAY(CLAY_ID("DiskChoiceHeader"), {
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(config.headerWidth),
                .height = CLAY_SIZING_GROW()
            },
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .padding = CLAY_PADDING_ALL(config.padding)
        }
    }) {
        CLAY(CLAY_ID("DiskChoiceLine"), {
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW() },
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 8
            }
        }) {
            CLAY(CLAY_ID("DiskChoiceText"), {
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIT(),
                        .height = CLAY_SIZING_GROW()
                    }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Choose:"), TEXT_MAIN(config.textColor));
            }
            
            CLAY(CLAY_ID("ChoiceSelector"), {
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

void UI_InfoLabel(InfoLabelConfig config, Clay_ElementId id) {
    CLAY(id, {
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
    CLAY(CLAY_ID("ProgramInfoHeader"), {
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(PROGRAM_INFO_HEADER_WIDTH),
                .height = CLAY_SIZING_GROW()
            }
        }
    }) {
        CLAY(CLAY_ID("ProgramInfoHeaderText"), {
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_GROW()
                },
                .padding = CLAY_PADDING_ALL(config.padding)
            }
        }) {
            CLAY_TEXT(config.title, TEXT_MAIN(config.textColor));
        }
    }
}

static void HandleExpandInteraction(Clay_ElementId elementId, 
                                     Clay_PointerData pointerData, 
                                     void* userData) {
    TreeNode* node = (TreeNode*)userData;
    
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        if (node && node->type == NODE_TYPE_DIRECTORY) {
            TreeNode_ToggleExpand(node);
        }
    }
}

void UI_TreeRow(TreeRowConfig config) {
    TreeNode* node = config.node;

    uint32_t nodeId = StringHash(node->fullPath.chars);

    CLAY(CLAY_IDI("TreeRow", nodeId), {
        .backgroundColor = Clay_Hovered() ? config.hoverColor : config.inactiveColor,
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_FIXED(config.rowHeight)
            }
        }
    }) {
        if (node->type == NODE_TYPE_DIRECTORY) {
            CLAY(CLAY_IDI("ExpandArrow", nodeId), {
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(EXPAND_ICON_WIDTH),
                        .height = CLAY_SIZING_GROW()
                    },
                    .padding = CLAY_PADDING_ALL(g_uiconfig.padding)
                }
            }) {
                Clay_OnHover(HandleExpandInteraction, (void*)node);
                
                Clay_String arrow = node->isExpanded ? 
                    CLAY_STRING("-") : CLAY_STRING("+");
                
                CLAY_TEXT(arrow, TEXT_MAIN(config.expandIconColor));
            }
        } else {
            CLAY(CLAY_IDI("FileIcon", nodeId),{
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(EXPAND_ICON_WIDTH),
                        .height = CLAY_SIZING_GROW()
                    }
                }
            }) {
            }
        }
        
        if (node->depth > 0) {
           CLAY(CLAY_IDI("Indent", nodeId), {
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(node->depth * config.indent),
                        .height = CLAY_SIZING_GROW()
                    }
                }
            }) {}
        }
        
        CLAY(CLAY_IDI("Name", nodeId), {
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(config.nameWidth),
                    .height = CLAY_SIZING_GROW()
                },
                .padding = CLAY_PADDING_ALL(g_uiconfig.padding)
            }
        }) {
            CLAY_TEXT(node->name, TEXT_MAIN(config.textColor));
        }
        
        CLAY(CLAY_IDI("Size", nodeId), {
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(config.sizeWidth),
                    .height = CLAY_SIZING_GROW()
                },
                .padding = CLAY_PADDING_ALL(g_uiconfig.padding)
            }
        }) {
            CLAY_TEXT(node->displaySize, TEXT_MAIN(config.textColor));
        }
    }
}

void UI_FileTreeView(FileTreeViewConfig config) {
    CLAY(CLAY_ID("FileTree"), {
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_GROW()
            },
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        },
        .backgroundColor = config.rowInactive
    }) {
        CLAY(CLAY_ID("FileTreeHeader"), {
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
            CLAY(CLAY_ID("NameColumn"), {
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(EXPAND_ICON_WIDTH + config.nameWidth),
                        .height = CLAY_SIZING_GROW()
                    },
                    .padding = CLAY_PADDING_ALL(g_uiconfig.padding)
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Name"), TEXT_MAIN(config.headerText));
            }
            CLAY(CLAY_ID("SizeColumn"), {
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(config.sizeWidth),
                        .height = CLAY_SIZING_GROW()
                    },
                    .padding = CLAY_PADDING_ALL(g_uiconfig.padding)
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Size"), TEXT_MAIN(config.headerText));
            }
        }
        
        CLAY(CLAY_ID("FileTreeContent"), {
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_GROW()
                },
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
            .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            FileTree_BuildVisibleList(config.tree);
            
            for (int i = 0; i < config.tree->visibleCount; i++) {
                TreeNode* node = config.tree->visibleNodes[i];
                
                UI_TreeRow((TreeRowConfig){
                    .node = node,
                    .textColor = config.rowText,
                    .hoverColor = config.rowHover,
                    .inactiveColor = config.rowInactive,
                    .expandIconColor = config.expandIconColor,
                    .indent = TREE_INDENT,
                    .rowHeight = config.rowHeight,
                    .nameWidth = config.nameWidth,
                    .sizeWidth = config.sizeWidth
                });
            }
        }
    }
}


void UI_HeaderBar(AppState* app_state) {
    CLAY(CLAY_ID("HeaderBar"), {
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
        
        CLAY(CLAY_ID("DiskInfoHeader"), {
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_GROW()
                }
            }
        }) {
            CLAY(CLAY_ID("SelectedDiskRow"), {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW() },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .childGap = 8
                }
            }) {
                CLAY(CLAY_ID("SelectedLabel"), {
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_GROW(),
                            .height = CLAY_SIZING_GROW()
                        },
                        .padding = CLAY_PADDING_ALL(g_uiconfig.padding)
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("Selected: "), TEXT_MAIN(g_colorscheme.text_primary));
                }

                CLAY(CLAY_ID("DriveName"), {
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_GROW(),
                            .height = CLAY_SIZING_GROW()
                        },
                        .padding = CLAY_PADDING_ALL(g_uiconfig.padding)
                    }
                }) {
                    CLAY_TEXT(app_state->availableDrives[app_state->selectedDrive], TEXT_MAIN(g_colorscheme.text_primary));
                }
            }
        }
        
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
    CLAY(CLAY_ID("Diagram"), {
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
    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_GROW()
            },
        },
        .backgroundColor = g_colorscheme.background,
    }) {
        UI_HeaderBar(app_state);
        
        UI_FileTreeView((FileTreeViewConfig){
            .tree = &app_state->fileTree,
            .headerBg = g_colorscheme.secondary,
            .headerText = g_colorscheme.text_primary,
            .rowText = g_colorscheme.text_primary,
            .rowHover = g_colorscheme.highlight,
            .rowInactive = g_colorscheme.background,
            .expandIconColor = g_colorscheme.text_secondary,
            .rowHeight = TABLE_ROW_HEIGHT,
            .nameWidth = FIELD_FILENAME_WIDTH,
            .sizeWidth = FIELD_SIZE_WIDTH
        });
        
        UI_DiagramPanel((PanelConfig){
            .title = CLAY_STRING(""),
            .bgColor = g_colorscheme.primary,
            .textColor = g_colorscheme.text_primary,
            .height = DIAGRAM_MENU_HEIGHT,
            .padding = g_uiconfig.padding
        });
    }
}