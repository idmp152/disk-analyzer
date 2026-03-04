#pragma once
#include "clay.h"
#include "app_state.h"
#include "data.h"


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
    TreeNode* node;
    Clay_Color textColor, hoverColor, inactiveColor, expandIconColor;
    int indent, rowHeight, nameWidth, sizeWidth;
} TreeRowConfig;

typedef struct {
    FileTree* tree;
    Clay_Color headerBg, headerText, rowText, rowHover, rowInactive, expandIconColor;
    int rowHeight, nameWidth, sizeWidth;
} FileTreeViewConfig;

typedef struct {
    Clay_String title;
    Clay_Color bgColor, textColor;
    int height, padding;
} PanelConfig;

void UI_DiskSelector(DiskSelectorConfig config);
void UI_InfoLabel(InfoLabelConfig config, Clay_ElementId id);
void UI_ProgramTitle(PanelConfig config);

void UI_HeaderBar(AppState* app_state);
void UI_DiagramPanel(PanelConfig config);
void UI_OuterContainer(AppState* app_state);

void UI_TreeRow(TreeRowConfig config);
void UI_FileTreeView(FileTreeViewConfig config);