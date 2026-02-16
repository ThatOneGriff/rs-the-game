#ifndef LOGIC_LAYER_H
#define LOGIC_LAYER_H

/* Headers */
#include <SDL3/SDL.h>       /// `SDL_Event`.
#include <stdbool.h>        /// `bool game_is_running`.

/// Purely for code clarity.
#define time_tick_ms unsigned long long int
#define time_tick_ns unsigned long long int
#define time_span_ms unsigned long long int
#define time_span_ns unsigned long long int


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
extern struct Logic_Layer logic_layer; /// Singleton.


/* Predef */

void init_logic_layer(int *const exit_code);
void free_logic_layer(void);


#endif /// LOGIC_LAYER_H