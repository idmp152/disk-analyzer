#include "app_state.h"

#define DROPDOWN_OPTIONS 64

Clay_String drives[3] = {
    CLAY_STRING("Drive C:"),
    CLAY_STRING("Drive D:"),
    CLAY_STRING("Drive F:")
};

void AppState_Initialize(AppState* app_state) {
    app_state->availableDrives = drives;
    app_state->driveCount = 3;
    app_state->selectedDrive = 0;
    app_state->dropdownOpen = false;

    TreeNodeArena_Init(&app_state->treeArena);
    FileTree_Init(&app_state->fileTree);

    DropdownOptionPool options = {
        .options = malloc(sizeof(DropdownOptionData)*DROPDOWN_OPTIONS),
        .count = 0
    };

    app_state->dropdownOptions = options;

    AppState_RefreshTree(app_state, "C:\\");
}

void AppState_Destroy(AppState* app_state) {
    FileTree_Clear(&app_state->fileTree);
    TreeNodeArena_Destroy(&app_state->treeArena);
    free(app_state->dropdownOptions.options);
}

void AppState_RefreshTree(AppState* state, const char* drivePath) {
    if (!state) return;
    
    TreeNodeArena_Clear(&state->treeArena);
    TreeNodeArena_Init(&state->treeArena);
    FileTree_Init(&state->fileTree);
    FileProvider_FillTree(&state->fileTree, &state->treeArena, drivePath);
}