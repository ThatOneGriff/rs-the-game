/* Related header */
#include "gameplay_events.h"

/* SDL3 */
#include <SDL3/SDL.h> /// Keyboard controls.
#include <stdio.h>    /// Text debug (TEMP).

/* Helper headers */
#include "../../debug.h"                   /// Error message printing.
#include "../../audio/audio_manager.h"     /// Audio
#include "../../audio/music_loader.h"      /// manipulation.
#include "../../graphics/fps.h"            /// FPS-based movement.
#include "../../graphics/graphics_layer.h" /// `RENDER_WIDTH/HEIGHT`.
#include "../../logic/global_events.h"     /// Global event processing.
#include "../../logic/logic_layer.h"       /// Key state.

/* Scene & components */
#include "../car.h"         /// Car position controlling.
#include "gameplay_scene.h" /// Gameplay scene manipulation.
#include "pause_screen.h"   /// Pause screen manipulation.


/* Predef */

void         process_gameplay_events   (struct Gameplay_Scene* scene);
static void _process_gameplay_keyboard (struct Gameplay_Scene* scene,      const SDL_Keycode event_key);
static void _process_pause_keyboard    (struct Pause_Screen* pause_screen, const SDL_Keycode event_key);
static void _process_gameplay_car_input(struct Car* car);


void process_gameplay_events(struct Gameplay_Scene* scene)
{
    if (! scene->pause_screen.is_open && scene->is_driving)
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
    /// TODO: param checking.
    /// Things that can happen both with and without options screen being open:
    switch(event_key)
    {
        case SDLK_ESCAPE:
            if      (! scene->pause_screen.is_open)
            {
                show_pause_screen(&scene->pause_screen);
                if (audio_manager.using_audio)
                    ma_sound_stop(&audio_manager.music);
            }
            else if (scene->pause_screen.is_open)
            {
                hide_pause_screen(&scene->pause_screen);
                if (audio_manager.using_audio)
                    ma_sound_start(&audio_manager.music);
            }
            break;
        
        case SDLK_M: /// TEMP: will be extended to playing next/previous track and pausing.
            play_random_music(&music_loader_gameplay);
            break;
    }

    /// Conditional redirection to 'Options' event handler:
    if (scene->pause_screen.is_open)
    {
        _process_pause_keyboard(&scene->pause_screen, event_key);
        return;
    }

    /// Gameplay button handling:
    switch(event_key)
    {
        case SDLK_UP:
            if (scene->start_tick == 0 && scene->crash_tick == 0)
            {
                scene->is_driving = true;
                scene->start_tick = logic_layer.curr_tick;
            }
            break;
        
        default:
            process_global_keyboard(event_key);
    }
    return;
}


static void _process_pause_keyboard(struct Pause_Screen* pause_screen, const SDL_Keycode event_key)
{
    /// TODO: param checking.

    /// Pause screen button handling:
    switch(event_key)
    {
        case SDLK_RETURN:
            if (pause_screen->close_button.is_focused || pause_screen->continue_button.is_focused)
            {
                hide_pause_screen(pause_screen);
                ma_sound_start(&audio_manager.music);
            }
            else if (pause_screen->quit_to_menu_button.is_focused)
                logic_layer.remain_in_scene = false;
            else if (pause_screen->quit_to_desktop_button.is_focused)
                logic_layer.game_is_running = false;
            break;
        
        case SDLK_UP:
            if (pause_screen->continue_button.is_focused)
            {
                pause_screen->close_button.is_focused    = true;
                pause_screen->continue_button.is_focused = false;
            }
            else if (pause_screen->quit_to_menu_button.is_focused)
            {
                pause_screen->continue_button.is_focused     = true;
                pause_screen->quit_to_menu_button.is_focused = false;
            }
            else if (pause_screen->quit_to_desktop_button.is_focused)
            {
                pause_screen->quit_to_menu_button.is_focused    = true;
                pause_screen->quit_to_desktop_button.is_focused = false;
            }
            break;
        
        case SDLK_DOWN:
            if (pause_screen->close_button.is_focused)
            {
                pause_screen->close_button.is_focused    = false;
                pause_screen->continue_button.is_focused = true;
            }
            else if (pause_screen->continue_button.is_focused)
            {
                pause_screen->continue_button.is_focused     = false;
                pause_screen->quit_to_menu_button.is_focused = true;
            }
            else if (pause_screen->quit_to_menu_button.is_focused)
            {
                pause_screen->quit_to_menu_button.is_focused    = false;
                pause_screen->quit_to_desktop_button.is_focused = true;
            }
            break;
        
        default:
            process_global_keyboard(event_key);
    }
    return;
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

    car->base_texture = (size_t)(2 + car->direction_x);
    /// NOTE: those nested `if-else`s are for overflow sanitizer compliance.
    /// - If turn recently started.
    if ((car->latest_turn_start != 0) && (logic_layer.curr_tick - car->latest_turn_start >= car->turn_smoothing_duration))
    {
        if (car->prev_turn_direction_x > 0)
            car->base_texture += (size_t)car->prev_turn_direction_x;
        else
        {
            if (car->base_texture <  (size_t)(-car->prev_turn_direction_x))
                car->base_texture = 0;
            else
                car->base_texture -= (size_t)(-car->prev_turn_direction_x);
        }
    }
    
    /// - If turn recently ended.
    else if ((car->latest_turn_end   != 0) && (logic_layer.curr_tick - car->latest_turn_end   <= car->turn_smoothing_duration))
    {
        if (car->prev_turn_direction_x > 0)
            car->base_texture += (size_t)car->prev_turn_direction_x;
        else
        {
            if (car->base_texture <  (size_t)(-car->prev_turn_direction_x))
                car->base_texture = 0;
            else
                car->base_texture -= (size_t)(-car->prev_turn_direction_x);
        }
    }
    
    /// Turning the texture
    int coord_based_diff = 0;
    if (car->coords.x <= 30)
        coord_based_diff =  2;
    else if (car->coords.x <= 60)
        coord_based_diff =  1;
    else if (car->coords.x + car->coords.w >= 210)
        coord_based_diff = -2;
    else if (car->coords.x + car->coords.w >= 180)
        coord_based_diff = -1;
    
    /// NOTE: again, integer overflow compliance.
    if (coord_based_diff > 0)
    {
        car->base_texture += (size_t)coord_based_diff;
        if (car->base_texture >= 5)
            car->base_texture = 4;
    }
    else
    {
        if (car->base_texture <  (size_t)(-coord_based_diff))
            car->base_texture = 0;
        else
            car->base_texture -= (size_t)(-coord_based_diff);
    }
    
    /// Moving the car across the screen.
    car->coords.x += (float)(car->direction_x) * (float)car->handling * (float)(FPS_manager.delta_ns) / SEC_IN_NS;
    return;
}