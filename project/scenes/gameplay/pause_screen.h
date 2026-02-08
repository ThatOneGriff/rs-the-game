#pragma once
#ifndef PAUSE_SCREEN_H
#define PAUSE_SCREEN_H

#include <SDL3/SDL.h>                        /// SDL things.
#include <stdbool.h>                         /// `bool is_open`.
#include "../../game_components/text/text.h" /// Text.
#include "../../game_components/ui/button.h" /// Buttons.
#include "../../game_components/ui/switch.h" /// Switches.
#include "../../logic/logic_layer.h"         /// `time_tick_ms`.


/* Struct */

struct Pause_Screen
{
    bool is_open;
    SDL_Texture* last_gameplay_frame;

    struct Texture pause_text;

    struct Button           close_button;
    struct Button        continue_button;
    struct Button    quit_to_menu_button;
    struct Button quit_to_desktop_button;
    struct Button*           curr_button;

    struct Texture version_text;
    time_tick_ms pause_open_tick;
};


/* Predef */

struct Pause_Screen init_pause_screen(int* exit_code);
void                free_pause_screen(struct Pause_Screen* target);
void show_pause_screen  (struct Pause_Screen* target);
void hide_pause_screen  (struct Pause_Screen* target);
void render_pause_screen(struct Pause_Screen* target);


#endif /// PAUSE_SCREEN_H