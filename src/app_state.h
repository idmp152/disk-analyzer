#pragma once
#include "clay.h"
#include "data.h"
#include "config.h"
#include "tree_arena.h"
#include "file_provider.h"
#include "renderers/raylib/raylib.h"

static const char FONT_PATH[] = "resources/RobotoMono-Regular.ttf";

typedef struct {
    int buttonIndex;
    int* selectedIndexPtr;
} DropdownOptionData;

typedef struct {
    DropdownOptionData* options;
    int count;
} DropdownOptionPool;

typedef struct {
    int selectedDrive;
    int hoveredFileIndex;
    
    FileTree fileTree;
    TreeNodeArena treeArena;

    Clay_String* availableDrives;
    int driveCount;

    DropdownOptionPool dropdownOptions;
    bool dropdownOpen;
} AppState;

void AppState_Initialize(AppState* app_state);
void AppState_Destroy(AppState* app_state);
void AppState_RefreshTree(AppState* state, const char* drivePath);