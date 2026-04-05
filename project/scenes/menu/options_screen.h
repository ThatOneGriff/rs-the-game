#ifndef OPTIONS_SCREEN_H
#define OPTIONS_SCREEN_H

#include <SDL3/SDL.h>                        /// SDL3.
#include <stdbool.h>                         /// `bool is_open`.
#include "../../game_components/ui/button.h" /// Buttons.
#include "../../game_components/ui/switch.h" /// Switches.


/* Struct */

struct Options_Screen
{
    bool is_open;
    
    SDL_Texture* last_menu_frame;
    struct Sprite options_text;

    struct Button close_button;
    struct Sprite    audio_text; struct Switch    audio_switch;
    struct Sprite      fps_text; struct Switch      fps_switch;
    struct Sprite show_fps_text; struct Switch show_fps_switch;
    struct Sprite  logging_text; struct Switch  logging_switch;
    
    void* curr_focus; /// Either a `Button` or a `Switch`.

    struct Sprite version_text;
};


/* Predef */

struct Options_Screen init_options_screen(int *const exit_code);
void                  free_options_screen(struct Options_Screen *const target);
void show_options_screen  (struct Options_Screen *const target);
void hide_options_screen  (struct Options_Screen *const target);
void render_options_screen(struct Options_Screen *const target);


#endif /// OPTIONS_SCREEN_H