#pragma once
#ifndef GAMEPLAY_EVENTS_H
#define GAMEPLAY_EVENTS_H

/* SDL3 */
#include <SDL3/SDL.h> /// Keyboard controls.

/* Helper headers */
#include "../../graphics/fps.h"            /// FPS-based movement.
#include "../../graphics/graphics_layer.h" /// `RENDER_WIDTH/HEIGHT`.
#include "../../logic/global_events.h"     /// Global event processing.
#include "../../logic/logic_layer.h"       /// Key state.

/* Scene & components */
#include "car.h"            /// Car position controlling.
#include "gameplay_scene.h" /// Gameplay scene manipulation.


/* Predef */

void  process_gameplay_events   (struct Gameplay_Scene* scene);
void _process_gameplay_keyboard (struct Gameplay_Scene* scene, const SDL_Keycode event_key);
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
    
    /// Input reading
    const bool* key_state = SDL_GetKeyboardState(NULL); /// Unlike the scancodes, this provides no delay - just like handling a car requires.
    car->direction_x = - key_state[SDL_SCANCODE_LEFT] + key_state[SDL_SCANCODE_RIGHT];
    /// Boundary processing
    if (car->direction_x != 0)
        car->prev_turn_direction_x = car->direction_x;
    if (car->direction_x == -1 && car->coords.x <= 0)
        car->direction_x = 0;
    else if (car->direction_x == 1 && car->coords.x + car->coords.w >= RENDER_WIDTH)
        car->direction_x = 0;
    
    /// Turn smoothing
    if (car->direction_x != 0 && car->latest_turn_start == 0)
    {
        car->latest_turn_start = logic_layer.curr_tick;
        car->latest_turn_end   = 0;
    }
    if (car->direction_x == 0 &car->latest_turn_end == 0)
    {
        car->latest_turn_end   = logic_layer.curr_tick;
        car->latest_turn_start = 0;
    }

    car->base_texture = 2 + car->direction_x;
    if      ((car->latest_turn_start != 0) && (logic_layer.curr_tick - car->latest_turn_start >= car->turn_smoothing_duration)) /// Turn recently started.
        car->base_texture += car->prev_turn_direction_x;
    else if ((car->latest_turn_end   != 0) && (logic_layer.curr_tick - car->latest_turn_end   <= car->turn_smoothing_duration)) /// Turn recently ended.
        car->base_texture += car->prev_turn_direction_x;
    
    /// Turning the texture
    if (car->coords.x <= 30)
        car->base_texture += 2;
    else if (car->coords.x <= 60)
        ++car->base_texture;
    else if (car->coords.x + car->coords.w >= 210)
        car->base_texture -= 2;
    else if (car->coords.x + car->coords.w >= 180)
        --car->base_texture;
    
    if (car->base_texture >= UINT_MAX - 100) /// Overflow happened; 100 is just a buffer for potential future changes,
        car->base_texture = 0;
    else if (car->base_texture >= 5)
        car->base_texture = 4;
    
    /* Moving car */
    car->coords.x += (float)(car->direction_x) * (float)car->handling * (float)(FPS_manager.delta_ns) / SEC_IN_NS;
    
    /*if (SDL_GetTicks() - car->latest_jump_tick >= (90.0 / 60.0 * 2)*1000 + 250) /// NOTE: should be synchronized with a track's BPM.
        car->coords.y = 45;
    else
        car->coords.y = 50;*/
    return;
}

#endif /// GAMEPLAY_EVENTS_H