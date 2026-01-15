#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "renderers/raylib/clay_renderer_raylib.c"
// #include "../shared-layouts/clay-video-demo.c"

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

static const char FONT_PATH[] = "resources/Roboto-Regular.ttf";

int main(void) {


    Clay_Raylib_Initialize(1024, 768, "Disk Analyzer", FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    uint64_t clayRequiredMemory = Clay_MinMemorySize();

    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    Clay_Dimensions clayDimensions = (Clay_Dimensions) {
        .width = GetScreenWidth(),
        .height = GetScreenHeight()
    };

    Clay_Initialize(clayMemory, clayDimensions, (Clay_ErrorHandler) { HandleClayErrors });

    Font fonts[1];
    fonts[0] = LoadFontEx(FONT_PATH, 48, NULL, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    SetExitKey(KEY_NULL);
    while(!WindowShouldClose()) {
        clayDimensions = (Clay_Dimensions) {
            .width = GetScreenWidth(),
            .height = GetScreenHeight()
        };

        Clay_SetLayoutDimensions(clayDimensions);

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
            .backgroundColor = { 43, 41, 51, 255 },
        }) {
            CLAY({
                .id = CLAY_ID("HeaderBar"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(),
                        .height = CLAY_SIZING_FIXED(32)
                    }
                },
                .backgroundColor = { 90, 90, 90, 255 }
            }) {}
            CLAY({
                .id = CLAY_ID("FileTree"),
                .layout = {
                    .sizing = layoutExpand
                }
            }) {}
            CLAY({
                .id = CLAY_ID("Diagram"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(),
                        .height = CLAY_SIZING_FIXED(250)
                    }
                },
                .backgroundColor = { 90, 90, 90, 255 }
            }) {}
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }

    Clay_Raylib_Close();
}