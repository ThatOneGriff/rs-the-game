/* Related header */
#include "menu_events.h"

/* SDL3 */
#include <SDL3/SDL.h> /// Keyboard processing.

/* Logic */
#include <stdbool.h>                   /// Bools.
#include "../../debug.h"               /// Error message printing.
#include "../../logic/global_events.h" /// Global event processing.
#include "../../logic/logic_layer.h"   /// Key state variable.

/* Scene & components */
#include "menu_scene.h"                   /// Menu scene manipulation.
#include "options_screen.h"               /// Options screen manipulation.
#include "../car_manager.h"               /// Minor car manager data pulling for 'Prev'/'Next' enabling.
#include "../../audio/audio_manager.h"    /// Audio
#include "../../audio/music_loader.h"     ///  manipulation.
#include "../../game_components/ui/button.h" /// Button manipulation.
#include "../../game_components/ui/switch.h" /// Switch manipulation.
#include "../../graphics/fps.h"           /// Frame cap control.

/// NOTE: not my proudest code.
/// I would've shortened a lot of things, if not for the time constraints.


/* Predef */

void         process_menu_events(void);
static void _process_menu_keyboard   (const SDL_Keycode event_key);
static void _process_options_keyboard(const SDL_Keycode event_key);


/* Body */

void process_menu_events(void)
{
    while (SDL_PollEvent(&logic_layer.event))
    {
        switch (logic_layer.event.type)
        {
        /// Key press.
        case SDL_EVENT_KEY_DOWN:
            logic_layer.screen_changed = true;
            _process_menu_keyboard(logic_layer.event.key.key);
            break;
        
        /// Other event.
        default:
            logic_layer.screen_changed = false;
            process_global_events(logic_layer.event);
            break;
        }
    }
    return;
}


static void _process_menu_keyboard(const SDL_Keycode event_key)
{
    logic_layer.screen_changed = true;
    int exit_code = EXIT_SUCCESS;

    /// Things that can happen both with and without options screen being open:
    switch(event_key)
    {
        case SDLK_ESCAPE:
            if    (! menu_scene.options_screen.is_open)
                show_options_screen(&menu_scene.options_screen);
            else if (menu_scene.options_screen.is_open)
                hide_options_screen(&menu_scene.options_screen);
            break;
        
        case SDLK_M: /// TEMP: will be extended to playing next/previous track and pausing.
            play_random_music(&music_loader_menu);
            logic_layer.screen_changed = false;
            break;
    }

    /// Conditional redirection to 'Options' event handler:
    if (menu_scene.options_screen.is_open)
    {
        _process_options_keyboard(event_key);
        return;
    }

    /// Menu button handling:
    switch(event_key)
    {
        case SDLK_RETURN:
            if      (menu_scene.prev_button.is_focused)
            {
                set_menu_car_info(get_prev_car(), &exit_code);
                if (players_car_manager.cur_car == 0)
                {
                    menu_scene.prev_button.is_focused = false;
                    menu_scene.next_button.is_focused = true;
                    menu_scene.curr_button = &menu_scene.next_button;
                }
            }
            else if (menu_scene.next_button.is_focused)
            {
                set_menu_car_info(get_next_car(), &exit_code);
                if (players_car_manager.cur_car == players_car_manager.car_count - 1)
                {
                    menu_scene.next_button.is_focused = false;
                    menu_scene.prev_button.is_focused = true;
                    menu_scene.curr_button = &menu_scene.prev_button;
                }
            }
            else if (menu_scene.play_button.is_focused)
                logic_layer.remain_in_scene = false;
            else if (menu_scene.options_button.is_focused)
                show_options_screen(&menu_scene.options_screen);
            else if (menu_scene.quit_button.is_focused)
                logic_layer.game_is_running = false;
            else
                logic_layer.screen_changed = false;
            break;

        case SDLK_UP:
            if (menu_scene.curr_button->up == NULL)
            {
                logic_layer.screen_changed = false;
                break;
            }
            menu_scene.curr_button->    is_focused = false;
            menu_scene.curr_button->up->is_focused = true;
            menu_scene.curr_button = menu_scene.curr_button->up;

            /// A little bit of hard-coding to not pick 'prev' button
            /// when player's car is 1st one [ID = 0]:
            if (menu_scene.curr_button == &menu_scene.prev_button
              && players_car_manager.cur_car == 0)
            {
                menu_scene.prev_button.is_focused = false;
                menu_scene.next_button.is_focused = true;
                menu_scene.curr_button = &menu_scene.next_button;
            }
            break;
        
        case SDLK_DOWN:
            if (menu_scene.curr_button->down == NULL)
            {
                logic_layer.screen_changed = false;
                break;
            }
            menu_scene.curr_button->      is_focused = false;
            menu_scene.curr_button->down->is_focused = true;
            menu_scene.curr_button = menu_scene.curr_button->down;
            break;

        case SDLK_LEFT:
            /// In menu
            if (! menu_scene.options_screen.is_open && players_car_manager.cur_car != 0)
            {
                if (menu_scene.curr_button->left == NULL)
                {
                    logic_layer.screen_changed = false;
                    break;
                }
                menu_scene.curr_button->      is_focused = false;
                menu_scene.curr_button->left->is_focused = true;
                menu_scene.curr_button = menu_scene.curr_button->left;
                break;
            }
            break;

        case SDLK_RIGHT:
            /// In menu
            if (! menu_scene.options_screen.is_open && players_car_manager.cur_car != players_car_manager.car_count - 1)
            {
                if (menu_scene.curr_button->right == NULL)
                {
                    logic_layer.screen_changed = false;
                    break;
                }
                menu_scene.curr_button->       is_focused = false;
                menu_scene.curr_button->right->is_focused = true;
                menu_scene.curr_button = menu_scene.curr_button->right;
                break;
            }
            break;

        default:
            logic_layer.screen_changed = false;
            process_global_keyboard(event_key);
            break;
    }

    if (exit_code == EXIT_FAILURE)
        print_error("`_process_menu_keyboard()`: an error code was thrown", NON_SDL_ERROR);
    return;
}


static void _process_options_keyboard(const SDL_Keycode event_key)
{
    logic_layer.screen_changed = true;
    int exit_code = EXIT_SUCCESS;

    /// Options screen button handling:
    switch(event_key)
    {
        case SDLK_RETURN:
            if (menu_scene.options_screen.audio_switch.is_focused)
            {
                audio_manager.using_audio = ! audio_manager.using_audio;
                change_switch_option(&menu_scene.options_screen.audio_switch);
                if (! audio_manager.using_audio)
                    ma_sound_stop(&audio_manager.music);
                else
                    play_random_music(&music_loader_menu);
            }
            else if (menu_scene.options_screen.close_button.is_focused)
            {
                hide_options_screen(&menu_scene.options_screen);
            }
            else if (menu_scene.options_screen.fps_switch.is_focused)
            {       
                ++curr_fps_cap_i;
                if (curr_fps_cap_i == 4)
                    curr_fps_cap_i = 0;
                set_fps_cap(fps_cap_options[curr_fps_cap_i]);
                change_switch_option(&menu_scene.options_screen.fps_switch);
            }
            break;

        case SDLK_UP:
            if      (menu_scene.options_screen.audio_switch.is_focused)
            {
                menu_scene.options_screen.audio_switch.is_focused = false;
                menu_scene.options_screen.close_button.is_focused = true;
            }
            else if (menu_scene.options_screen.fps_switch.is_focused)
            {
                menu_scene.options_screen.fps_switch.  is_focused = false;
                menu_scene.options_screen.audio_switch.is_focused = true;
            }
            else
                logic_layer.screen_changed = false;
            break;
        
        case SDLK_DOWN:
            if (menu_scene.options_screen.close_button.is_focused)
            {
                menu_scene.options_screen.close_button.is_focused = false;
                menu_scene.options_screen.audio_switch.is_focused = true;
            }
            else if (menu_scene.options_screen.audio_switch.is_focused)
            {
                menu_scene.options_screen.audio_switch.is_focused = false;
                menu_scene.options_screen.fps_switch.is_focused   = true;
            }
            else
                logic_layer.screen_changed = false;
            break;

        default:
            logic_layer.screen_changed = false;
            process_global_keyboard(event_key);
            break;
    }

    if (exit_code == EXIT_FAILURE)
        print_error("`_process_options_keyboard()`: an error code was thrown", NON_SDL_ERROR);
    return;
}