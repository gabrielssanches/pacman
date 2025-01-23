#include "pacman.h"

#include <threads.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static struct game_context gctx = {0};

void game_state_change(struct game_context *gctx, enum state_name next_state) {
    if (next_state < NUMBER_OF_STATES) {
        gctx->state_next = gctx->states[next_state];
    }
}

struct game_state gamestate_none = {
    .init = NULL,
    .deinit = NULL,
    .enter = NULL,
    .exit = NULL,
    .update = NULL,
    .draw = NULL
};

static void UpdateDrawFrame(void) {
    if (gctx.state_current != gctx.state_next) {
        if (gctx.state_current->exit != NULL) {
            gctx.state_current->exit(&gctx);
        }
        if (gctx.state_next->enter != NULL) {
            gctx.state_next->enter(&gctx);
        }
        gctx.state_current = gctx.state_next;
    }
    gctx.state_current->update(&gctx);
    gctx.state_current->draw(&gctx);
}

int main(int argc, char *argv[]) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);

    // pacman original screen resolution is 224x288
    // the tile size is 8x8 px, so 28x36 tiles
    // this clone is intended to be played at 1080p vertical
    // so scaling the tile size to 32x32 px fits :D
    InitWindow(1080, 1920, "pacman clone by gabrielssanches");

    //DisableCursor();

    gctx.state_current = &gamestate_none;
    gctx.state_next = &gamestate_game; // FIXME: gamestate_load
    gctx.states[STATE_LOAD] = &gamestate_load;
    gctx.states[STATE_INTRO_RAYLIB] = &gamestate_intro_raylib;
    gctx.states[STATE_GAME] = &gamestate_game;

    // init all states
    for (int i = 0; i < NUMBER_OF_STATES; i++) {
        if (gctx.states[i]->init != NULL) {
            gctx.states[i]->init(&gctx);
        }
    }

#if defined(PLATFORM_WEB)
    EM_ASM({ window.onkeydown = function(e) { return !(e.keyCode == 32); }; });
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    while (!gctx.quit && !WindowShouldClose()) {   // Detect window close button or ESC key
        UpdateDrawFrame();
    }
#endif

    // deinit all states
    for (int i = 0; i < NUMBER_OF_STATES; i++) {
        if (gctx.states[i]->deinit != NULL) {
            gctx.states[i]->deinit(&gctx);
        }
    }

   if (gctx.audio_init) {
       CloseAudioDevice();
   }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}

