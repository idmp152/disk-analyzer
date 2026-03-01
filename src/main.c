#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "renderers/raylib/clay_renderer_raylib.c"
#include "config.h"
#include "ui_components.h"

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

void LoadFonts(Font* fonts) {
    fonts[0] = LoadFontEx(FONT_PATH, 48, NULL, 400);
    SetTextureFilter(fonts[g_uiconfig.font_id].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
}

void HandleMouseInput_BeforeRender() {
    Vector2 mouse = GetMousePosition();
    Vector2 scroll = GetMouseWheelMoveV();
    
    Clay_SetPointerState(
        (Clay_Vector2){mouse.x, mouse.y}, 
        IsMouseButtonDown(MOUSE_BUTTON_LEFT)
    );
    Clay_UpdateScrollContainers(
        true, 
        (Clay_Vector2){scroll.x, scroll.y}, 
        GetFrameTime()
    );
}

void HandleMouseInput_AfterRender(AppState* state) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Clay_ElementId selector = Clay_GetElementId(CLAY_STRING("ChoiceSelector"));
        state->dropdownOpen = Clay_PointerOver(selector) ? !state->dropdownOpen : false;
    }
}

int main(void) {
    Clay_Raylib_Initialize(g_uiconfig.window_width, g_uiconfig.window_height, "Disk Analyzer", FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    uint64_t clayRequiredMemory = Clay_MinMemorySize();

    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    Clay_Dimensions clayDimensions = (Clay_Dimensions) {
        .width = GetScreenWidth(),
        .height = GetScreenHeight()
    };

    Clay_Initialize(clayMemory, clayDimensions, (Clay_ErrorHandler) { HandleClayErrors });

    AppState app_state;
    AppState_Initialize(&app_state);

    Font fonts[1];
    LoadFonts(fonts);

    SetExitKey(KEY_NULL);

    while(!WindowShouldClose()) {
        clayDimensions = (Clay_Dimensions) {
            .width = GetScreenWidth(),
            .height = GetScreenHeight()
        };

        Clay_SetLayoutDimensions(clayDimensions);

        HandleMouseInput_BeforeRender();

        Clay_BeginLayout();

        Clay_Sizing layoutExpand = {
            .width = CLAY_SIZING_GROW(),
            .height = CLAY_SIZING_GROW()
        };

        UI_RenderUI(&app_state);

        HandleMouseInput_AfterRender(&app_state);

        Clay_RenderCommandArray renderCommands = Clay_EndLayout();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }

    AppState_Destroy(&app_state);
    Clay_Raylib_Close();
}