#pragma once
#ifndef MENU_CONTROLS_H
#define MENU_CONTROLS_H

#include <SDL3/SDL.h> /// Keyboard processing.


/* Predef */

void  process_menu_events(void);
void _process_menu_keyboard(const SDL_Keycode event_key);


/* Body */

void process_menu_events(void)
{
    while (SDL_PollEvent(&logic_layer.event))
    {
        switch (logic_layer.event.type)
        {
        /* Key press */
        case SDL_EVENT_KEY_DOWN:
            printf("0.7");
            _process_menu_keyboard(logic_layer.event.key.key);
            break;
        
        /* Quit */
        case SDL_EVENT_QUIT:
            logic_layer.game_is_running = false;
            break;
        }
    }
}


void _process_menu_keyboard(const SDL_Keycode event_key)
{
    switch(event_key)
    {
        case SDLK_D:
            printf("1");
            logic_layer.remain_in_scene = false;
            break;
    }
}

#endif /// MENU_CONTROLS_H