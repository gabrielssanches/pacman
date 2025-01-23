#ifndef PACMAN_H 
#define PACMAN_H 

#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib_ext.h"

#ifdef _DEBUG
#define DEBUG_PRINT(fmt, ...) \
    do { \
        fprintf(stderr, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

#define RESOLUTION_WIDTH 1080
#define RESOLUTION_HEIGHT 1920

typedef struct game_context game_context_t;

enum state_name {
    STATE_LOAD,
    STATE_INTRO_RAYLIB,
    STATE_GAME,
    NUMBER_OF_STATES,
};

enum soundfx {
    SFX_MAX,
};

enum theme {
    THEME_MAX,
};

typedef void (*init_func_t)(game_context_t *gctx);

struct game_state {
    init_func_t init;
    init_func_t deinit;
    init_func_t enter;
    init_func_t exit;
    init_func_t update;
    init_func_t draw;
};

struct game_context {
    bool audio_init;
    bool quit;
    struct game_state *state_current;
    struct game_state *state_next;
    struct game_state *states[NUMBER_OF_STATES];
    int level_sel;
    Sound sfx[SFX_MAX];
    Music theme[THEME_MAX];
    Music *theme_playing;
};

void game_state_change(struct game_context *gctx, enum state_name next_state);

extern struct game_state gamestate_load;
extern struct game_state gamestate_intro_raylib;
extern struct game_state gamestate_game;

#endif // PACMAN_H 
