#pragma once
#include "clay.h"
#include "app_state.h"
#include "data.h"


typedef struct {
    Clay_String text;
    Clay_Color textColor;
    int width;
} TableColumnConfig;

typedef struct {
    File* file;
    Clay_Color textColor, hoverColor, inactiveColor;
    int filenameWidth, sizeWidth, createdWidth, modifiedWidth, rowHeight;
} TableRowConfig;

typedef struct {
    Clay_String* drives;
    int driveCount;
    int* selectedIndex;
    DropdownOptionPool* options;
    bool isOpen;
    Clay_Color bgColor, hoverColor, textColor, borderColor;
    int headerWidth, menuWidth, padding;
} DiskSelectorConfig;

typedef struct {
    Clay_String label;
    Clay_String value;
    Clay_Color textColor;
    int padding;
} InfoLabelConfig;


typedef struct {
    File* files;
    int fileCount;
    Clay_Color headerBg, headerText, rowText, rowHover, rowInactive;
    int rowHeight;
    int filenameWidth, sizeWidth, createdWidth, modifiedWidth;
} FileTableConfig;

typedef struct {
    Clay_String title;
    Clay_Color bgColor, textColor;
    int height, padding;
} PanelConfig;


void UI_TableColumn(TableColumnConfig config);
void UI_TableRow(TableRowConfig config);
void UI_FileTableHeader(FileTableConfig config);
void UI_FileTableContent(FileTableConfig config);
void UI_FileTable(FileTableConfig config);

void UI_DiskSelector(DiskSelectorConfig config);
void UI_InfoLabel(InfoLabelConfig config);
void UI_ProgramTitle(PanelConfig config);

void UI_HeaderBar(AppState* app_state);
void UI_DiagramPanel(PanelConfig config);
void UI_OuterContainer(AppState* app_state);

void UI_RenderUI(AppState* app_state);