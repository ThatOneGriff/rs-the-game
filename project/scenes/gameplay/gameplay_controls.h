#pragma once
#ifndef GAMEPLAY_CONTROLS_H
#define GAMEPLAY_CONTROLS_H

#include <SDL3/SDL.h> /// Keyboard controls.
#include "car.h"                     /// Car controls.
#include "../../graphics/fps.h"      /// FPS-based movement.
#include "../../logic/logic_layer.h" /// Key state.


void process_gameplay_input(struct Car* car)
{
    if (car == NULL)
    {
        print_error("`process_gameplay_input()`: `car` arg is `NULL`", NON_SDL_ERROR);
        return;
    }
    /// Player movement
    logic_layer.key_state = SDL_GetKeyboardState(NULL);
    size_t prev_car_direction_x = car->direction_x;
    car->direction_x = - logic_layer.key_state[SDL_SCANCODE_LEFT] + logic_layer.key_state[SDL_SCANCODE_RIGHT];
    if (car->direction_x == -1 && car->coords.x <= 0)
        car->direction_x = 0;
    else if (car->direction_x == 1 && car->coords.x + car->coords.w >= RENDER_WIDTH)
        car->direction_x = 0;
    
    car->base_texture = 2 + (car->direction_x + prev_car_direction_x);
    if (car->coords.x <= 80)
        ++car->base_texture;
    else if (car->coords.x + car->coords.w >= 160)
        --car->base_texture;
    
    if (car->base_texture >= UINT_MAX - 100) /// Overflow happened; 100 just to account for potential future changes,
        car->base_texture = 0;
    else if (car->base_texture >= 5)
        car->base_texture = 4;
    
    /*if (SDL_GetTicks() - car->latest_jump_tick >= (90.0 / 60.0 * 2)*1000 + 250) /// NOTE: can be synchronized with a track's BPM.
        car->coords.y = 45;
    else
        car->coords.y = 50;*/
    return;
}

#endif /// GAMEPLAY_CONTROLS_H