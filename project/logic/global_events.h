#pragma once
#ifndef GLOBAL_EVENTS_H
#define GLOBAL_EVENTS_H

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_Event`, `SDL_Keycode`.

/* C headers */
#include <stdbool.h> /// For `logic_layer` manipulation.
#include <stdio.h>   /// I/O.

/* Helper headers */
#include "logic_layer.h"           /// Exiting the game.
#include "../debug.h"              /// Some controls should only be available `#ifdef DEBUG`.
#include "../graphics/fps.h"       /// Frame cap control.
#include "../helpers/helpers.h"    /// Colored output.
#include "../resources.h"          /// Hot resource reload.
#include "../audio/music_loader.h" /// Music manipulation.


/* Predef */

void  process_global_events  (const SDL_Event   event);
void _process_global_keyboard(const SDL_Keycode event_key);


/* Body */

/// Receives an `event`, because it gets called from scene-related event functions
/// (i.e. doesn't read an event on its own because of the *lower priority*).
void process_global_events(const SDL_Event event)
{
    switch (event.type)
    {
    /// Quit
    case SDL_EVENT_QUIT:
        logic_layer.game_is_running = false;
        break;
    }
}


void _process_global_keyboard(const SDL_Keycode event_key)
{
    int exit_code = EXIT_SUCCESS;
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
        else /// Inf FPS cap (=> turned off)
        {
            textcolor(GRAY);
            printf("- [FPS cap] Disabled\n");
            textcolor(WHITE);
            rem_fps_cap();
        }
        break;
    
    case SDLK_R:
        _load_global_resources(&exit_code);
        if (exit_code == EXIT_SUCCESS)
            print_success("Hot resource reload; switch to next scene and back to see the effect");
        else /// First multi-line error btw.
            print_error("Hot resource reload:\n- Scene reloading will result in a crash;\n- You may try to fix invalid paths and reload again", NON_SDL_ERROR);
        break;
    }
}

#endif /// GLOBAL_EVENTS_H