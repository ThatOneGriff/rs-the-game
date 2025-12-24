#pragma once
#ifndef MENU_CONTROLS_H
#define MENU_CONTROLS_H

#include <SDL3/SDL.h> /// Keyboard processing.
#include "../../logic/logic_layer.h" /// Key state.
#include "../../logic/input.h"       /// Global keyboard processing.


/* Predef */

void  process_menu_event(const SDL_Event event);
void _process_menu_keyboard(const SDL_Keycode event_key);


/* Body */

void process_menu_event(const SDL_Event event)
{
    switch (event.type)
    {
    /* Key press */
    case SDL_EVENT_KEY_DOWN:
        _process_menu_keyboard(event.key.key);
        break;
    }
}


void _process_menu_keyboard(const SDL_Keycode event_key)
{
    switch(event_key)
    {
        case SDLK_D:
            logic_layer.remain_in_scene = false;
            break;
        default:
            _process_global_keyboard(event_key);
    }
}

#endif /// MENU_CONTROLS_H