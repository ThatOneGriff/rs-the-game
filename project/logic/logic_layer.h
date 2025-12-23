#pragma once
#ifndef LOGIC_LAYER_H
#define LOGIC_LAYER_H

#include <SDL3/SDL.h> /// `SDL_Event`.
#include <stdbool.h>  /// `bool game_is_running`.
#include "../debug.h" ///  Error printing.


/* Predef */

struct Logic_Layer;
static struct Logic_Layer logic_layer; /// Singleton.
void _init_logic_layer(int* exit_code);
void _free_logic_layer(void);


/* Struct */

struct Logic_Layer
{
    SDL_Event event;
    bool game_is_running;
};

/// Doesn't check for accidental double initialization.
void _init_logic_layer(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`_init_logic_layer()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    logic_layer.game_is_running = true;
    *exit_code = EXIT_SUCCESS;
    return;
}

void _free_logic_layer(void)
{
    logic_layer.game_is_running = false;
    return;
}

#endif /// LOGIC_LAYER_H