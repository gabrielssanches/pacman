#include "pacman.h"

static Texture2D sprite_sheet;
static Texture2D sprite_characters;
static int tile;
static float game_time;
static Camera2D camera = { 0 };

static void game_enter(struct game_context *gctx) {
    sprite_sheet = LoadTexture("resources/sprites/map.png");
    sprite_characters = LoadTexture("resources/sprites/characters.png");
    tile = 1;
    game_time = GetTime();

    camera.target = Vector2Zero();
    camera.offset = Vector2Zero();
    camera.rotation = 0.0f;
    camera.zoom = (10*GetScreenHeight()/RESOLUTION_HEIGHT)/10.0f;
}

static void game_update(struct game_context *gctx) {
    float now_ts = GetTime();
    if ((now_ts - game_time) > 1.0f) {
        game_time = now_ts;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 mouse_delta = GetMouseDelta();
        camera.offset = Vector2Add(camera.offset, mouse_delta);
    }
    //camera.zoom = (10*GetScreenHeight()/RESOLUTION_HEIGHT)/10.0f;
    //camera.zoom += 0.1f*GetMouseWheelMove();
}
static int tilemap[952] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,5,5,5,5,5,5,5,5,5,5,6,1,5,5,5,5,5,5,5,5,5,5,5,5,6,4,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,20,18,18,22,0,20,18,18,18,22,0,7,4,0,20,18,18,18,22,0,20,18,18,22,0,7,4,0,7,24,24,4,0,7,24,24,24,4,0,7,4,0,7,24,24,24,4,0,7,24,24,4,0,7,4,0,21,5,5,23,0,21,5,5,5,23,0,21,23,0,21,5,5,5,23,0,21,5,5,23,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,20,18,18,22,0,20,22,0,20,18,18,18,18,18,18,22,0,20,22,0,20,18,18,22,0,7,4,0,21,5,5,23,0,7,4,0,21,5,5,6,1,5,5,23,0,7,4,0,21,5,5,23,0,7,4,0,0,0,0,0,0,7,4,0,0,0,0,7,4,0,0,0,0,7,4,0,0,0,0,0,0,7,2,8,8,8,8,3,0,7,17,18,18,22,0,7,4,0,20,18,18,19,4,0,13,8,8,8,8,12,0,0,0,0,0,10,0,7,1,5,5,23,0,21,23,0,21,5,5,6,4,0,10,0,0,0,0,0,0,0,0,0,0,10,0,7,4,0,0,0,0,0,0,0,0,0,0,7,4,0,10,0,0,0,0,0,0,0,0,0,0,10,0,7,4,0,13,8,8,11,11,8,8,3,0,7,4,0,10,0,0,0,0,0,8,8,8,8,8,9,0,21,23,0,10,0,0,0,0,0,0,10,0,21,23,0,14,8,8,8,8,8,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,3,0,20,22,0,10,0,0,0,0,0,0,10,0,20,22,0,13,8,8,8,8,8,0,0,0,0,0,10,0,7,4,0,14,8,8,8,8,8,8,9,0,7,4,0,10,0,0,0,0,0,0,0,0,0,0,10,0,7,4,0,0,0,0,0,0,0,0,0,0,7,4,0,10,0,0,0,0,0,0,0,0,0,0,10,0,7,4,0,20,18,18,18,18,18,18,22,0,7,4,0,10,0,0,0,0,0,15,8,8,8,8,9,0,21,23,0,21,5,5,6,1,5,5,23,0,21,23,0,14,8,8,8,8,16,4,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,20,18,18,22,0,20,18,18,18,22,0,7,4,0,20,18,18,18,22,0,20,18,18,22,0,7,4,0,21,5,6,4,0,21,5,5,5,23,0,21,23,0,21,5,5,5,23,0,7,1,5,23,0,7,4,0,0,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,0,0,7,17,18,22,0,7,4,0,20,22,0,20,18,18,18,18,18,18,22,0,20,22,0,7,4,0,20,18,19,1,5,23,0,21,23,0,7,4,0,21,5,5,6,1,5,5,23,0,7,4,0,21,23,0,21,5,6,4,0,0,0,0,0,0,7,4,0,0,0,0,7,4,0,0,0,0,7,4,0,0,0,0,0,0,7,4,0,20,18,18,18,18,19,17,18,18,22,0,7,4,0,20,18,18,19,17,18,18,18,18,22,0,7,4,0,21,5,5,5,5,5,5,5,5,23,0,21,23,0,21,5,5,5,5,5,5,5,5,23,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,19 };

static void game_draw(struct game_context *gctx) {
    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode2D(camera);

    // draw a tile at a time
    for (int row = 0; row < 34; row++) {
        for (int col = 0; col < 28; col++) {
            tile = tilemap[col + (row*28)];
            Rectangle tile_rect = { tile*32.0f, 0.0f, 32, 32};
            DrawTextureRec(sprite_sheet, tile_rect, (Vector2){ col*32.0f, row*32.0f }, WHITE);  // Draw part of the texture
        }
    }

    // drawp pacman
    int pac = 1;
    Rectangle pacman_rect = { (pac*4 + 1)*32.0f, 0.0f, 32, 32};
    DrawTextureRec(sprite_characters, pacman_rect, (Vector2){ 0.0f, 0.0f }, WHITE);
    pacman_rect.x = (pac*4 + 2)*32.0f;
    DrawTextureRec(sprite_characters, pacman_rect, (Vector2){ 32.0f, 0.0f }, WHITE);
    pacman_rect.x = (pac*4 + 3)*32.0f;
    DrawTextureRec(sprite_characters, pacman_rect, (Vector2){ 0.0f, 32.0f }, WHITE);
    pacman_rect.x = (pac*4 + 4)*32.0f;
    DrawTextureRec(sprite_characters, pacman_rect, (Vector2){ 32.0f, 32.0f }, WHITE);

    // draw screen outline
    Rectangle screen_outline = { 0, 0, 0, 0};
    screen_outline.width = RESOLUTION_WIDTH;
    screen_outline.height = RESOLUTION_HEIGHT;
    DrawRectangleLinesEx(screen_outline, 2.0, RED);

    EndMode2D();

    DrawText(TextFormat("%ix%i@%i zoom:%0.3f", GetScreenWidth(), GetScreenHeight(), GetFPS(), camera.zoom), 30, 30, 30, BLUE);

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
