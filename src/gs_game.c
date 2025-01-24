#include "pacman.h"

static Texture2D sprite_sheet;
static Texture2D sprite_characters;
static int tile;
static float game_time;
static Camera2D camera = { 0 };

struct pacman {
    Vector2 vel;
    Vector2 pos;
};
struct pacman pacman;

// character movement general rules
// - movement direction is always defined by UP,DOWN,LEFT,RIGHT
// - movement can be changed to the current allowed directions
// - allowed directions are defined on character spawning and on cross points
// - movement is computed a pixel at a time, indenpendent of the speed.
//   e.g.: x0 = 10, x1=14. Then needed checks are made at 10,11,12,13,14
//   this is dump coding at the expense of cpu cycles
//
// pacman:
// - movement direction can be set to NONE
// - always starts moving to the LEFT, on position x,y
// - considered at cross point if near enough (2px distance max)
// - held back one screen refresh if just ate a dot

// line grid:
// line x0,y0 x1,y1
// have an array of all lines

// ai:
// - unique target for each ghost
// - target is defined as a point. points are within lines, in a grid
// - to reach target, calculate shortest path by weighting points to target. lowest is chosen

// pacman dies:
// - in mode MODE_A: when a ghost is in the same point as pacman

// pacman eats:
// - when pacman is in the same point as the eatable (dot/pill/fruit)
// 
static void game_enter(struct game_context *gctx) {
    sprite_sheet = LoadTexture("resources/sprites/map.png");
    sprite_characters = LoadTexture("resources/sprites/characters.png");
    tile = 1;
    game_time = GetTime();

    camera.target = Vector2Zero();
    camera.offset = Vector2Zero();
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    //camera.zoom = (10*GetScreenHeight()/RESOLUTION_HEIGHT)/10.0f;

    pacman.pos = Vector2Zero();
    pacman.pos.x = 48;
    pacman.pos.y = 144;
    pacman.vel = Vector2Zero();
}

struct cross_point {
    Vector2 pos;
};

struct line {
    Vector2 p0;
    Vector2 p1;
};
static int mouse_hold = 0;
static int mouse_undo = 0;
static int mouse_line_new = 0;
static Vector2 mouse_line_p0;
static Vector2 mouse_line_p1;

static struct cross_point mouse_cross_point[100];
static int mouse_point_count = 0;

static Vector2 gridGetMousePosition(void) {
    Vector2 grid_pos = GetMousePosition();
    grid_pos.x = 16*((int)grid_pos.x/16);
    grid_pos.y = 16*((int)grid_pos.y/16);
    return grid_pos;
}

static void game_update(struct game_context *gctx) {
    float now_ts = GetTime();
    if ((now_ts - game_time) > 1.0f) {
        game_time = now_ts;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && mouse_undo == 0) {
        mouse_undo = 1;
        if (mouse_point_count > 0) {
            mouse_point_count--;
        }
    }
    if (IsMouseButtonUp(MOUSE_BUTTON_RIGHT) && mouse_undo == 1) {
        mouse_undo = 0;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mouse_hold == 0) {
        mouse_hold = 1;
        mouse_line_new = 0;
        mouse_line_p0 = gridGetMousePosition();
    }
    if (IsMouseButtonUp(MOUSE_BUTTON_LEFT) && mouse_hold == 0) {
        mouse_line_p0 = gridGetMousePosition();
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mouse_hold == 1) {
        mouse_line_p1 = gridGetMousePosition();
    }
    if (IsMouseButtonUp(MOUSE_BUTTON_LEFT) && mouse_hold == 1) {
        mouse_hold = 0;
        mouse_line_new = 1;
        mouse_line_p1 = gridGetMousePosition();
        mouse_cross_point[mouse_point_count].pos = mouse_line_p0;
        mouse_point_count++;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 mouse_delta = GetMouseDelta();
        camera.offset = Vector2Add(camera.offset, mouse_delta);
    }
    //camera.zoom = (10*GetScreenHeight()/RESOLUTION_HEIGHT)/10.0f;
    //camera.zoom += 0.1f*GetMouseWheelMove();

    if (IsKeyDown(KEY_DOWN)) {
        pacman.vel.y = 1;
        pacman.vel.x = 0;
    }
    if (IsKeyDown(KEY_UP)) {
        pacman.vel.y = -1;
        pacman.vel.x = 0;
    }
    if (IsKeyDown(KEY_LEFT)) {
        pacman.vel.y = 0;
        pacman.vel.x = -1;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        pacman.vel.y = 0;
        pacman.vel.x = 1;
    }
    pacman.pos = Vector2Add(pacman.pos, pacman.vel);
}

static int tilemap[952] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,5,5,5,5,5,5,5,5,5,5,6,1,5,5,5,5,5,5,5,5,5,5,5,5,6,4,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,20,18,18,22,0,20,18,18,18,22,0,7,4,0,20,18,18,18,22,0,20,18,18,22,0,7,4,0,7,24,24,4,0,7,24,24,24,4,0,7,4,0,7,24,24,24,4,0,7,24,24,4,0,7,4,0,21,5,5,23,0,21,5,5,5,23,0,21,23,0,21,5,5,5,23,0,21,5,5,23,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,20,18,18,22,0,20,22,0,20,18,18,18,18,18,18,22,0,20,22,0,20,18,18,22,0,7,4,0,21,5,5,23,0,7,4,0,21,5,5,6,1,5,5,23,0,7,4,0,21,5,5,23,0,7,4,0,0,0,0,0,0,7,4,0,0,0,0,7,4,0,0,0,0,7,4,0,0,0,0,0,0,7,2,8,8,8,8,3,0,7,17,18,18,22,0,7,4,0,20,18,18,19,4,0,13,8,8,8,8,12,0,0,0,0,0,10,0,7,1,5,5,23,0,21,23,0,21,5,5,6,4,0,10,0,0,0,0,0,0,0,0,0,0,10,0,7,4,0,0,0,0,0,0,0,0,0,0,7,4,0,10,0,0,0,0,0,0,0,0,0,0,10,0,7,4,0,13,8,8,11,11,8,8,3,0,7,4,0,10,0,0,0,0,0,8,8,8,8,8,9,0,21,23,0,10,0,0,0,0,0,0,10,0,21,23,0,14,8,8,8,8,8,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,3,0,20,22,0,10,0,0,0,0,0,0,10,0,20,22,0,13,8,8,8,8,8,0,0,0,0,0,10,0,7,4,0,14,8,8,8,8,8,8,9,0,7,4,0,10,0,0,0,0,0,0,0,0,0,0,10,0,7,4,0,0,0,0,0,0,0,0,0,0,7,4,0,10,0,0,0,0,0,0,0,0,0,0,10,0,7,4,0,20,18,18,18,18,18,18,22,0,7,4,0,10,0,0,0,0,0,15,8,8,8,8,9,0,21,23,0,21,5,5,6,1,5,5,23,0,21,23,0,14,8,8,8,8,16,4,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,20,18,18,22,0,20,18,18,18,22,0,7,4,0,20,18,18,18,22,0,20,18,18,22,0,7,4,0,21,5,6,4,0,21,5,5,5,23,0,21,23,0,21,5,5,5,23,0,7,1,5,23,0,7,4,0,0,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,0,0,0,7,17,18,22,0,7,4,0,20,22,0,20,18,18,18,18,18,18,22,0,20,22,0,7,4,0,20,18,19,1,5,23,0,21,23,0,7,4,0,21,5,5,6,1,5,5,23,0,7,4,0,21,23,0,21,5,6,4,0,0,0,0,0,0,7,4,0,0,0,0,7,4,0,0,0,0,7,4,0,0,0,0,0,0,7,4,0,20,18,18,18,18,19,17,18,18,22,0,7,4,0,20,18,18,19,17,18,18,18,18,22,0,7,4,0,21,5,5,5,5,5,5,5,5,23,0,21,23,0,21,5,5,5,5,5,5,5,5,23,0,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,17,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,19 };


static const Vector2 line_grid_offset = { 48.0f, 144.0f };
static const struct line line_grid[2] = {
    { .p0 = { .x = 0.0f, .y = 0.0f}, .p1 = { .x = 0.0f, .y = 224.0f} },
    { .p0 = { .x = 100.0f, .y = 0.0f}, .p1 = { .x = 0.0f, .y = 100.0f} }
};


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

    // draw pacman
    int pac = 1;
    Rectangle pacman_rect = { (pac*4 + 1)*32.0f, 0.0f, 32, 32};
    Vector2 pacman_rect_draw = { pacman.pos.x -32.0f, pacman.pos.y -32.0f};

    DrawTextureRec(sprite_characters, pacman_rect, pacman_rect_draw, WHITE);

    pacman_rect.x = (pac*4 + 2)*32.0f;
    pacman_rect_draw.x += 32.0f;
    DrawTextureRec(sprite_characters, pacman_rect, pacman_rect_draw, WHITE);

    pacman_rect.x = (pac*4 + 3)*32.0f;
    pacman_rect_draw.x -= 32.0f;
    pacman_rect_draw.y += 32.0f;
    DrawTextureRec(sprite_characters, pacman_rect, pacman_rect_draw, WHITE);

    pacman_rect.x = (pac*4 + 4)*32.0f;
    pacman_rect_draw.x += 32.0f;
    DrawTextureRec(sprite_characters, pacman_rect, pacman_rect_draw, WHITE);

    // draw line grid
    for (int line = 0; line < 2; line++) {
        struct line line_sel = line_grid[line];
        line_sel.p0 = Vector2Add(line_sel.p0, line_grid_offset);
        line_sel.p1 = Vector2Add(line_sel.p1, line_grid_offset);
        DrawLineEx(line_sel.p0, line_sel.p1, 2.0f, GREEN);
    }

    for (int point = 0; point < mouse_point_count; point++) {
        DrawCircleV(mouse_cross_point[point].pos, 2.0f, GREEN);
    }
    if (mouse_hold == 0) {
        DrawCircleV(mouse_line_p0, 2.0f, GRAY);
    }

    // draw screen outline
    Rectangle screen_outline = { 0, 0, 0, 0};
    screen_outline.width = RESOLUTION_WIDTH;
    screen_outline.height = RESOLUTION_HEIGHT;
    DrawRectangleLinesEx(screen_outline, 2.0, RED);

    // draw tile grid
    for (int row = 0; row <= 34; row++) {
        Vector2 line_start = { 0.0f, row*32.0f };
        Vector2 line_stop = { RESOLUTION_WIDTH, row*32.0f };
        DrawLineEx(line_start, line_stop, 2.0f, GRAY);
    }
    for (int col = 0; col <= 28; col++) {
        Vector2 line_start = { col*32.0f, 0.0f };
        Vector2 line_stop = { col*32.0f, RESOLUTION_HEIGHT };
        DrawLineEx(line_start, line_stop, 2.0f, GRAY);
    }

    EndMode2D();

    Vector2 mouse_pos = GetMousePosition();
    mouse_pos = Vector2Subtract(mouse_pos, line_grid_offset);
    mouse_pos.x = 16*((int)mouse_pos.x/16);
    mouse_pos.y = 16*((int)mouse_pos.y/16);
    DrawText(TextFormat("%ix%i@%i zoom:%0.3f mouse:%0.3f,%0.3f", GetScreenWidth(), GetScreenHeight(), GetFPS(), camera.zoom, mouse_pos.x, mouse_pos.y), 30, 30, 30, BLUE);

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
