/* Related header */
#include "logic_layer.h"

/* Headers */
#include <SDL3/SDL.h>       /// `SDL_Event`.
#include <SDL3/SDL_timer.h> /// Tick measurement.
#include <stdbool.h>        /// `bool game_is_running`.
#include <stdlib.h>         /// `EXIT_SUCCESS`/`EXIT_FAILURE`.
#include "../debug.h"       ///  Error printing.


/* Body */

/// Doesn't check for accidental double initialization.
void _init_logic_layer(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`_init_logic_layer()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    logic_layer.game_is_running = true;
    logic_layer.remain_in_scene = true;
    logic_layer.curr_scene = NULL;
    logic_layer.curr_tick      = SDL_GetTicks(); /// Temp value. Will be updated every frame.
    logic_layer.real_tick_diff = 0; /// For pause reasons.
    *exit_code = EXIT_SUCCESS;
    return;
}

void _free_logic_layer(void)
{
    logic_layer.game_is_running = false;
    logic_layer.remain_in_scene = false;
    logic_layer.curr_scene = NULL;
    return;
}