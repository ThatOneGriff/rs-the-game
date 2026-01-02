#pragma once
#ifndef GAMEPLAY_CONTROLS_H
#define GAMEPLAY_CONTROLS_H

#include <SDL3/SDL.h> /// Keyboard controls.
#include "car.h"                     /// Car controls.
#include "gameplay_scene.h" /// Gameplay scene manipulation.
#include "../../graphics/fps.h"      /// FPS-based movement.
#include "../../logic/logic_layer.h" /// Key state.
#include "../../logic/input.h"       /// Global keyboard processing.


/* Predef */

void  process_gameplay_events  (struct Gameplay_Scene* scene);
void _process_gameplay_keyboard(struct Gameplay_Scene* scene, const SDL_Keycode event_key);
void _process_gameplay_car_input(struct Car* car);


void process_gameplay_events(struct Gameplay_Scene* scene)
{
    _process_gameplay_car_input(scene->car_ptr);
    while (SDL_PollEvent(&logic_layer.event))
    {
        switch (logic_layer.event.type)
        {
        /// Key press.
        case SDL_EVENT_KEY_DOWN:
            _process_gameplay_keyboard(scene, logic_layer.event.key.key);
            break;
        /// Other event.
        default:
            process_global_events(logic_layer.event);
        }
    }
}


void _process_gameplay_keyboard(struct Gameplay_Scene* scene, const SDL_Keycode event_key)
{
    UNUSED(scene); /// I'm not sure if it ever will be used. Let's keep it for now.
    switch(event_key)
    {
        case SDLK_RETURN:
            logic_layer.remain_in_scene = false;
            break;
        
        default:
            _process_global_keyboard(event_key);
    }
}


void _process_gameplay_car_input(struct Car* car)
{
    if (car == NULL)
    {
        print_error("`process_gameplay_car_input()`: `car` arg is `NULL`", NON_SDL_ERROR);
        return;
    }
    
    /* Input reading */
    logic_layer.key_state = SDL_GetKeyboardState(NULL);
    size_t prev_car_direction_x = car->direction_x;
    car->direction_x = - logic_layer.key_state[SDL_SCANCODE_LEFT] + logic_layer.key_state[SDL_SCANCODE_RIGHT];
    
    /* Boundary processing */
    if (car->direction_x == -1 && car->coords.x <= 0)
        car->direction_x = 0;
    else if (car->direction_x == 1 && car->coords.x + car->coords.w >= RENDER_WIDTH)
        car->direction_x = 0;
    
    /* Shifting texture */
    car->base_texture = 2 + (car->direction_x + prev_car_direction_x);
    if (car->coords.x <= 80)
        ++car->base_texture;
    else if (car->coords.x + car->coords.w >= 160)
        --car->base_texture;
    
    if (car->base_texture >= UINT_MAX - 100) /// Overflow happened; 100 just to account for potential future changes,
        car->base_texture = 0;
    else if (car->base_texture >= 5)
        car->base_texture = 4;
    
    /* Moving car */
    car->coords.x += car->direction_x * car->handling * (FPS_manager.delta_ns / SEC_IN_NS);
    
    /*if (SDL_GetTicks() - car->latest_jump_tick >= (90.0 / 60.0 * 2)*1000 + 250) /// NOTE: can be synchronized with a track's BPM.
        car->coords.y = 45;
    else
        car->coords.y = 50;*/
    return;
}

#endif /// GAMEPLAY_CONTROLS_H