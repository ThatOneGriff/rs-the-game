#pragma once
#ifndef LOGIC_LAYER_H
#define LOGIC_LAYER_H

/* Headers */
#include <SDL3/SDL.h>       /// `SDL_Event`.
#include <SDL3/SDL_timer.h> /// Tick measurement. Only needed once upon initialization. Maybe there's a better way?
#include <stdbool.h>        /// `bool game_is_running, *key_state`.
#include "../debug.h"       ///  Error printing.

/// Purely for code clarity.
#define time_tick_ns unsigned long long int
#define time_tick_ms unsigned long long int
#define time_span_ns unsigned long long int
#define time_span_ms unsigned long long int


/* Predef */

struct Logic_Layer;
static struct Logic_Layer logic_layer; /// Singleton.
void _init_logic_layer(int* exit_code);
void _free_logic_layer(void);


/* Struct */

struct Logic_Layer
{
    bool  game_is_running;

    bool  remain_in_scene;
    void* curr_scene;

    SDL_Event event;

    time_tick_ms curr_tick;
    time_tick_ms real_tick_diff;
};

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

#endif /// LOGIC_LAYER_H