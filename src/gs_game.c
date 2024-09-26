#include "pacman.h"

static void game_enter(struct game_context *gctx) {
}

static void game_update(struct game_context *gctx) {
}

static void game_draw(struct game_context *gctx) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawRectangle(100, 100, 100, 100, RED);

    EndDrawing();
}

struct game_state gamestate_game = {
    .init = NULL,
    .deinit = NULL,
    .enter = game_enter,
    .exit = NULL,
    .update = game_update,
    .draw = game_draw
};
