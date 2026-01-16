#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "renderers/raylib/clay_renderer_raylib.c"

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

typedef struct {
    Clay_String name;
    Clay_String created_at;
    Clay_String modified_at;
    Clay_String size;
} File;

typedef struct {
    int buttonIndex;
    int* selectedIndexPtr;
} DropdownOptionData;


static const char FONT_PATH[] = "resources/RobotoMono-Regular.ttf";
static const int MAIN_PADDING = 10;
static const int MAIN_FONT_SIZE = 20;
static const int MAIN_FONT_ID = 0;
static const int MAIN_BORDER_WIDTH = 2;

static const int DEFAULT_WINDOW_WIDTH = 1200;
static const int DEFAULT_WINDOW_HEIGHT = 700;

//TODO: Rename (e.g. accent, highlight)
static const Clay_Color light = { 90, 90, 90, 255 };
static const Clay_Color light2 = { 80, 80, 80, 255 };
static const Clay_Color light3 = { 110, 110, 110, 255 };
static const Clay_Color dark = { 43, 41, 51, 255 };
static const Clay_Color highlight = { 70, 70, 70, 255 };
static const Clay_Color white = { 255, 255 , 255, 255 };

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

// SAMPLE DATA TODO: Replace with real data parsing from MFT
File fileArray[10] = { 
    {.name = CLAY_STRING("Q4_Financial_Report.xlsx"), .created_at = CLAY_STRING("2023-10-15 09:30:22"), .modified_at = CLAY_STRING("2023-12-05 16:45:10"), .size = CLAY_STRING("8.7 MB") },
    {.name = CLAY_STRING("vacation_photos.zip"), .created_at = CLAY_STRING("2022-07-22 14:20:05"), .modified_at = CLAY_STRING("2022-07-22 14:20:05"), .size = CLAY_STRING("245.3 MB") },
    {.name = CLAY_STRING("project_proposal.pdf"), .created_at = CLAY_STRING("2024-01-08 11:05:17"), .modified_at = CLAY_STRING("2024-01-10 09:15:33"), .size = CLAY_STRING("3.2 MB") },
    {.name = CLAY_STRING("system_backup.iso"), .created_at = CLAY_STRING("2024-02-28 03:00:00"), .modified_at = CLAY_STRING("2024-02-28 03:45:12"), .size = CLAY_STRING("4876.5 MB") },
    {.name = CLAY_STRING("meeting_notes.txt"), .created_at = CLAY_STRING("2024-03-12 15:40:21"), .modified_at = CLAY_STRING("2024-03-12 16:20:08"), .size = CLAY_STRING("0.02 MB") },
    {.name = CLAY_STRING("database_dump.sql"), .created_at = CLAY_STRING("2024-01-25 23:15:00"), .modified_at = CLAY_STRING("2024-02-15 23:15:00"), .size = CLAY_STRING("124.8 MB") },
    {.name = CLAY_STRING("presentation_final.pptx"), .created_at = CLAY_STRING("2023-11-30 10:12:45"), .modified_at = CLAY_STRING("2023-12-02 17:30:19"), .size = CLAY_STRING("15.6 MB") },
    {.name = CLAY_STRING("software_installer.exe"), .created_at = CLAY_STRING("2024-02-10 08:00:00"), .modified_at = CLAY_STRING("2024-02-10 08:00:00"), .size = CLAY_STRING("89.3 MB") },
    {.name = CLAY_STRING("research_data.csv"), .created_at = CLAY_STRING("2023-09-05 13:25:40"), .modified_at = CLAY_STRING("2024-01-18 11:10:25"), .size = CLAY_STRING("12.4 MB") },
    {.name = CLAY_STRING("user_manual.docx"), .created_at = CLAY_STRING("2023-12-12 16:40:11"), .modified_at = CLAY_STRING("2024-02-28 10:05:29"), .size = CLAY_STRING("2.1 MB") },
};

Clay_String drives[3] = {
    CLAY_STRING("Drive C:"),
    CLAY_STRING("Drive D:"),
    CLAY_STRING("Drive F:")
};


void TableColumnComponent(Clay_String text, Clay_Color textColor, int width) {
    CLAY({
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(width),
                .height = CLAY_SIZING_GROW()
            },
            .padding = CLAY_PADDING_ALL(MAIN_PADDING)
        }
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = MAIN_FONT_ID,
            .fontSize = MAIN_FONT_SIZE,
            .textColor = textColor
        }));
    }
}

void TableRowComponent(File* file, Clay_Color textColor, Clay_Color hoverColor, Clay_Color inactiveColor) {
    CLAY({
        .backgroundColor = Clay_Hovered() ? hoverColor : inactiveColor,
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
                .height = CLAY_SIZING_FIXED(TABLE_ROW_HEIGHT)
            }
        }
    }) {
        TableColumnComponent(file->name, textColor, FIELD_FILENAME_WIDTH);
        TableColumnComponent(file->size, textColor, FIELD_SIZE_WIDTH);
        TableColumnComponent(file->created_at, textColor, FIELD_CREATED_AT_WIDTH);
        TableColumnComponent(file->modified_at, textColor, FIELD_MODIFIED_AT_WIDTH);
    }
}

void HandleDropdownOptionInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData) {
    DropdownOptionData* dropdownData = (DropdownOptionData*)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        *(dropdownData->selectedIndexPtr) = dropdownData->buttonIndex;
    }
}

void DropDownMenuItemComponent(Clay_String text, Clay_Color textColor, Clay_Color hoverColor, Clay_Color inactiveColor, DropdownOptionData* dropdownData) {
    CLAY({
        .backgroundColor = Clay_Hovered() ? hoverColor : inactiveColor,
        .layout = {
            .padding = CLAY_PADDING_ALL(MAIN_PADDING),
            .sizing = {
                .width = CLAY_SIZING_GROW()
            }
        }
    }) {
        Clay_OnHover(HandleDropdownOptionInteraction, dropdownData);
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = MAIN_FONT_ID,
            .fontSize = MAIN_FONT_SIZE,
            .textColor = textColor
        }));
    }
}

int main(void) {
    Clay_Raylib_Initialize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Disk Analyzer", FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    uint64_t clayRequiredMemory = Clay_MinMemorySize();

    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    Clay_Dimensions clayDimensions = (Clay_Dimensions) {
        .width = GetScreenWidth(),
        .height = GetScreenHeight()
    };

    Clay_Initialize(clayMemory, clayDimensions, (Clay_ErrorHandler) { HandleClayErrors });

    Font fonts[1];
    fonts[0] = LoadFontEx(FONT_PATH, 48, NULL, 400);
    SetTextureFilter(fonts[MAIN_FONT_ID].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    SetExitKey(KEY_NULL);

    int selectedDrive = 0;
    bool renderDropdown = false;

    while(!WindowShouldClose()) {
        // TODO: refactor, split initializations by functions, refactor common components
        clayDimensions = (Clay_Dimensions) {
            .width = GetScreenWidth(),
            .height = GetScreenHeight()
        };

        Clay_SetLayoutDimensions(clayDimensions);

        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseWheelMoveV();
        Clay_SetPointerState((Clay_Vector2) { mousePosition.x, mousePosition.y }, IsMouseButtonDown(0));
        Clay_UpdateScrollContainers(true, (Clay_Vector2) { scrollDelta.x, scrollDelta.y }, GetFrameTime());

        Clay_BeginLayout();

        Clay_Sizing layoutExpand = {
            .width = CLAY_SIZING_GROW(),
            .height = CLAY_SIZING_GROW()
        };

        CLAY({
            .id = CLAY_ID("OuterContainer"),
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
            },
            .backgroundColor = dark,
        }) {
            CLAY({
                .id = CLAY_ID("HeaderBar"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(),
                        .height = CLAY_SIZING_FIXED(HEADER_BAR_HEIGHT)
                    },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT
                },
                .backgroundColor = light,
                .border = { .width = { .betweenChildren = MAIN_BORDER_WIDTH }, .color = light2 }
            }) {
                CLAY({
                    .id = CLAY_ID("DiskChoiceHeader"),
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(DISK_CHOICE_HEADER_WIDTH),
                            .height = CLAY_SIZING_GROW()
                        },
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .padding = CLAY_PADDING_ALL(MAIN_PADDING)
                    }
                }) {
                    CLAY({
                        .id = CLAY_ID("DiskChoiceLine"),
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_GROW()
                            },
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
                            CLAY_TEXT(CLAY_STRING("Choose:"), CLAY_TEXT_CONFIG({
                                .fontId = MAIN_FONT_ID,
                                .fontSize = MAIN_FONT_SIZE,
                                .textColor = white
                            }));
                        }
                        CLAY({
                            .id = CLAY_ID("ChoiceSelector"),
                            .layout = {
                                .sizing = {
                                    .width = CLAY_SIZING_GROW(),
                                    .height = CLAY_SIZING_GROW()
                                },
                                .padding = { .left = 8 }
                            },
                            .backgroundColor = light3
                        }) { 
                            CLAY_TEXT(drives[selectedDrive], CLAY_TEXT_CONFIG({
                                .fontId = MAIN_FONT_ID,
                                .fontSize = MAIN_FONT_SIZE,
                                .textColor = white
                            }));
                            
                            if (renderDropdown) {
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
                                        .sizing = {
                                            .width = CLAY_SIZING_FIXED(DISK_CHOICE_MENU_WIDTH)
                                        }
                                    },
                                    .backgroundColor = dark,
                                    .border = { 
                                        .width = { 
                                            .left = MAIN_BORDER_WIDTH,
                                            .right = MAIN_BORDER_WIDTH,
                                            .bottom = MAIN_BORDER_WIDTH
                                        },
                                        .color = white 
                                    }
                                }) {
                                    DropdownOptionData dropdownDataArray[3];
                                    for (int i = 0; i < 3; ++i) { //TODO: refactor, create global context
                                        dropdownDataArray[i].buttonIndex = i;
                                        dropdownDataArray[i].selectedIndexPtr = &selectedDrive;
                                        DropDownMenuItemComponent(drives[i], white, highlight, dark, dropdownDataArray + i);
                                    }
                                }
                            }
                        }  
                    }
                }
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
                            .sizing = {
                                .width = CLAY_SIZING_GROW()
                            },
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .childGap = 8
                        }
                    }) {
                        CLAY({
                            .layout = {
                                .sizing = {
                                    .width = CLAY_SIZING_GROW(),
                                    .height = CLAY_SIZING_GROW()
                                },
                                .padding = CLAY_PADDING_ALL(MAIN_PADDING)
                            }
                        }) {
                            CLAY_TEXT(CLAY_STRING("Selected:"), CLAY_TEXT_CONFIG({
                                .fontId = MAIN_FONT_ID,
                                .fontSize = MAIN_FONT_SIZE,
                                .textColor = white
                            }));
                        }
                        CLAY({
                            .layout = {
                                .sizing = {
                                    .width = CLAY_SIZING_GROW(),
                                    .height = CLAY_SIZING_GROW()
                                },
                                .padding = CLAY_PADDING_ALL(MAIN_PADDING)
                            }
                        }) {
                            CLAY_TEXT(drives[selectedDrive], CLAY_TEXT_CONFIG({
                                .fontId = MAIN_FONT_ID,
                                .fontSize = MAIN_FONT_SIZE,
                                .textColor = white
                            }));
                        }
                    }
                }
                CLAY({
                    .id = CLAY_ID("ProgramInfoHeader"),
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(PROGRAM_INFO_HEADER_WIDTH),
                            .height= CLAY_SIZING_GROW()
                        }
                    }
                }) {
                    CLAY({
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_GROW(),
                                .height = CLAY_SIZING_GROW()
                            },
                            .padding = CLAY_PADDING_ALL(MAIN_PADDING)
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Disk Analyzer"), CLAY_TEXT_CONFIG({
                            .fontId = MAIN_FONT_ID,
                            .fontSize = MAIN_FONT_SIZE,
                            .textColor = white
                        }));
                    }
                }
            }
            CLAY({
                .id = CLAY_ID("FileTree"),
                .layout = {
                    .sizing = layoutExpand,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM
                }
            }) {
                CLAY({
                    .id = CLAY_ID("FileTreeHeader"),
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_GROW(),
                            .height = CLAY_SIZING_FIXED(TABLE_ROW_HEIGHT)
                        }
                    },
                    .backgroundColor = light2,
                    .border = { .width = { .betweenChildren = MAIN_BORDER_WIDTH }, .color = light }
                }) {
                    TableColumnComponent(CLAY_STRING("Filename"), white, FIELD_FILENAME_WIDTH);
                    TableColumnComponent(CLAY_STRING("Size"), white, FIELD_SIZE_WIDTH);
                    TableColumnComponent(CLAY_STRING("Created At"), white, FIELD_CREATED_AT_WIDTH);
                    TableColumnComponent(CLAY_STRING("Modified At"), white, FIELD_MODIFIED_AT_WIDTH);
                }
                CLAY({
                    .id = CLAY_ID("FileTreeContent"),
                    .layout = {
                        .sizing = layoutExpand,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    },
                    .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
                }) {
                    for (int i = 0; i < 10; ++i) { // TODO: refactor
                        TableRowComponent(fileArray + i, white, highlight, dark);
                    }
                }
            }
            CLAY({
                .id = CLAY_ID("Diagram"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(),
                        .height = CLAY_SIZING_FIXED(DIAGRAM_MENU_HEIGHT)
                    }
                },
                .backgroundColor = light
            }) {}
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
           if (renderDropdown) {
                renderDropdown = false;
           } else {
                renderDropdown = Clay_PointerOver(Clay_GetElementId(CLAY_STRING("ChoiceSelector")));
           }
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }

    Clay_Raylib_Close();
}