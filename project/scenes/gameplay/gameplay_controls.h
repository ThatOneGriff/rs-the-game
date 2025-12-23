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
    float direction_dx = - logic_layer.key_state[SDL_SCANCODE_LEFT] + logic_layer.key_state[SDL_SCANCODE_RIGHT];
    if (direction_dx < 0 && car->coords.x <= 0)
        car->direction_x = 0;
    else if (direction_dx > 0 && car->coords.x + car->coords.w >= RENDER_WIDTH)
        car->direction_x = 0;
    else
        car->direction_x = direction_dx;
    return;
}

#endif /// GAMEPLAY_CONTROLS_H