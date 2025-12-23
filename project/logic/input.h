#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h> /// SDL3.

#include <stdbool.h> /// For `logic_layer` manipulation.
#include <stdio.h>   /// I/O.

#include "../graphics/fps.h"    /// Frame cap control.
#include "../helpers/helpers.h" /// Colored output.
#include "logic_layer.h"        /// Exiting the game.
//#include "game_components/scene.h"


/* Predef */

void  process_events(void);
//void _process_input (void);
void _process_keyboard(const SDL_Keycode event_key);


/* Body */

void process_events(void)
{
    //_process_input(); /// Seems logical to put it into `SDL_EVENT_KEY_DOWN`. However, this means a fraction-of-second input lag.
    while (SDL_PollEvent(&logic_layer.event))
    {

        switch (logic_layer.event.type)
        {
        /// Key press
        case SDL_EVENT_KEY_DOWN:
            _process_keyboard(logic_layer.event.key.key);
            break;
        
        /// Quit
        case SDL_EVENT_QUIT:
            logic_layer.game_is_running = false;
            break;
        }
    }
}

/// NOTE: `Joker_Demo` leftovers. Will be useful.
/*void _process_input(void)
{
    /// Player movement
    logic_layer.key_state = SDL_GetKeyboardState(NULL);

    scene.player.direction.y = - logic_layer.key_state[SDL_SCANCODE_UP]   + logic_layer.key_state[SDL_SCANCODE_DOWN];
    scene.player.direction.x = - logic_layer.key_state[SDL_SCANCODE_LEFT] + logic_layer.key_state[SDL_SCANCODE_RIGHT];
    
    const float fraction_of_16ms = (float)(graphics_layer.delta) / (reference_frame_time_60fps);
    float x_move = (float)scene.player.direction.x * scene.player.velocity * fraction_of_16ms;
    float y_move = (float)scene.player.direction.y * scene.player.velocity * fraction_of_16ms;

    if (x_move != 0 && y_move != 0) /// So that moving in a diagonal isn't faster.
    {
        x_move /= 1.414;
        y_move /= 1.414;
    }

    scene.player.rect.x += x_move;
    if (scene.player.rect.x < 0)
        scene.player.rect.x = 0;
    else if (scene.player.rect.x + scene.player.rect.w > WINDOW_WIDTH)
        scene.player.rect.x =      WINDOW_WIDTH - scene.player.rect.w;
    
    scene.player.rect.y += y_move;
    if (scene.player.rect.y < 0)
        scene.player.rect.y = 0;
    else if (scene.player.rect.y + scene.player.rect.h > WINDOW_HEIGHT)
        scene.player.rect.y =      WINDOW_HEIGHT - scene.player.rect.h;
}*/


void _process_keyboard(const SDL_Keycode event_key)
{
    int exit_code = EXIT_SUCCESS;
    switch(event_key)
    {
    /// Frame cap on / off
    case SDLK_F:
        FPS_manager.fps_capped = ! FPS_manager.fps_capped;
        if (FPS_manager.fps_capped)
        {
            textcolor(GRAY);
            printf("+ [FPS cap] %u\n", 60);
            textcolor(WHITE);
            set_fps_cap(60);
        }
        else
        {
            textcolor(GRAY);
            printf("- [FPS cap] Disabled\n");
            textcolor(WHITE);
            rem_fps_cap();
        }
        break; /// WARNING: with an esoteric-enough fall-through,
               /// case(smth1)'s `EXIT_FAILURE` may be overshadowed by case(smth2)'s `EXIT_SUCCESS`.
    
    /// Exit
    case SDLK_ESCAPE:
        logic_layer.game_is_running = false;
        break;
    }

    if (exit_code == EXIT_FAILURE)
        program_exit(exit_code);
}

#endif /// INPUT_H