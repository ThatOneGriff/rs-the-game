#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h> /// SDL3.

#include <stdbool.h> /// For `logic_layer` manipulation.
#include <stdio.h>   /// I/O.

#include "../graphics/fps.h"    /// Frame cap control.
#include "../helpers/helpers.h" /// Colored output.
#include "logic_layer.h"        /// Exiting the game.


/* Predef */

/*void  process_global_events(void);
void _process_global_keyboard(const SDL_Keycode event_key);


/// Body

void process_global_events(void)
{
    while (SDL_PollEvent(&logic_layer.event))
    {
        switch (logic_layer.event.type)
        {
        /// Key press
        case SDL_EVENT_KEY_DOWN:
            _process_global_keyboard(logic_layer.event.key.key);
            break;
        
        /// Quit
        case SDL_EVENT_QUIT:
            logic_layer.game_is_running = false;
            break;
        
        //default:
            /// NOTE: `process_menu_events()` and `process_gameplay_input()` need to have the same type & args.
        }
    }
}


void _process_global_keyboard(const SDL_Keycode event_key)
{
    switch(event_key)
    {
    /// Frame cap on / off
    case SDLK_F:
        ++curr_fps_cap_i;
        if (curr_fps_cap_i == 5)
            curr_fps_cap_i = 0;
        
        if (curr_fps_cap_i != 4) /// Non-inf FPS cap
        {
            textcolor(GRAY);
            printf("+ [FPS cap] %u\n", fps_cap_options[curr_fps_cap_i]);
            textcolor(WHITE);
            set_fps_cap(fps_cap_options[curr_fps_cap_i]);
        }
        else /// Inf FPS cap
        {
            textcolor(GRAY);
            printf("- [FPS cap] Disabled\n");
            textcolor(WHITE);
            rem_fps_cap();
        }
        break;
        

    /// WARNING: with an esoteric-enough fall-through,
    /// case(smth1)'s `EXIT_FAILURE` may be overshadowed by case(smth2)'s `EXIT_SUCCESS`.
    }
}*/

#endif /// INPUT_H