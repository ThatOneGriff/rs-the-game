/* Related header */
#include "logic_layer.h"

/* Headers */
#include <SDL3/SDL.h>       /// `SDL_Event`.
#include <SDL3/SDL_timer.h> /// Tick measurement.
#include <stdbool.h>        /// `bool game_is_running`.
#include <stdlib.h>         /// `EXIT_SUCCESS`/`EXIT_FAILURE`.
#include <string.h>         /// `memset()`.
#include "../debug.h"       ///  Error printing.


/* Variables */

struct Logic_Layer logic_layer = {0};


/* Predef */

void init_logic_layer(int *const exit_code);
void free_logic_layer(void);


/* Body */

/// Doesn't check for accidental double initialization.
void init_logic_layer(int *const exit_code)
{
    if (exit_code == NULL)
        print_warning("`_init_logic_layer()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    logic_layer.screen_changed  = true;
    logic_layer.force_render    = true; /// For the initial render to happen.
    logic_layer.game_is_running = true;
    logic_layer.remain_in_scene = true;
    logic_layer.curr_scene = NULL;
    logic_layer.curr_tick      = SDL_GetTicks(); /// Temp value. Will be updated every frame.
    logic_layer.real_tick_diff = 0; /// For pause reasons.
    *exit_code = EXIT_SUCCESS;
    return;
}

void free_logic_layer(void)
{
    memset(&logic_layer, 0, sizeof logic_layer);
    return;
}