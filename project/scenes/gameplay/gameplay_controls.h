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

    float car_direction = - logic_layer.key_state[SDL_SCANCODE_LEFT] + logic_layer.key_state[SDL_SCANCODE_RIGHT];
    if (car_direction == 0)
        return;
    if ((car_direction == -1 && car->texture.rect.x <= 0)
     || (car_direction ==  1 && car->texture.rect.x + car->texture.rect.w >= RENDER_WIDTH))
        return;
    
    car->texture.rect.x += car_direction * car->handling * (FPS_manager.delta_ns / SEC_IN_NS);
    return;
}

#endif /// GAMEPLAY_CONTROLS_H