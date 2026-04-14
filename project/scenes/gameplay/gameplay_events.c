/* Related header */
#include "gameplay_events.h"

/* SDL3 */
#include <SDL3/SDL.h> /// Keyboard controls.

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

void         process_gameplay_events   (void);
static void _process_gameplay_keyboard (const SDL_Keycode event_key);
static void _process_pause_keyboard    (const SDL_Keycode event_key);
static void _process_gameplay_car_input(void);


void process_gameplay_events(void)
{
    if (! gameplay_scene.pause_screen.is_open && gameplay_scene.is_driving)
        _process_gameplay_car_input();
    
    while (SDL_PollEvent(&logic_layer.event))
    {
        switch (logic_layer.event.type)
        {
        /// Key press.
        case SDL_EVENT_KEY_DOWN:
            _process_gameplay_keyboard(logic_layer.event.key.key);
            break;
        /// Other event.
        default:
            process_global_events(logic_layer.event);
        }
    }
}


void _process_gameplay_keyboard(const SDL_Keycode event_key)
{
    /// Things that can happen both with and without options screen being open:
    switch(event_key)
    {
        case SDLK_ESCAPE:
            if    (! gameplay_scene.pause_screen.is_open)
            {
                show_pause_screen(&gameplay_scene.pause_screen);
                if (audio_manager.using_audio)
                    ma_sound_stop(&audio_manager.music);
                graphics_layer.screen_changed = true; /// For the initial render to happen.
            }
            else if (gameplay_scene.pause_screen.is_open)
            {
                hide_pause_screen(&gameplay_scene.pause_screen);
                if (audio_manager.using_audio)
                    ma_sound_start(&audio_manager.music);
            }
            break;
        
        case SDLK_M: /// TEMP: will be extended to playing next/previous track and pausing.
            if (audio_manager.audio_is_valid)
                play_random_music(&music_loader_gameplay);
            break;
    }

    /// Conditional redirection to 'Options' event handler:
    if (gameplay_scene.pause_screen.is_open)
    {
        _process_pause_keyboard(event_key);
        return;
    }

    /// Gameplay button handling:
    switch(event_key)
    {
        case SDLK_UP:
            if (gameplay_scene.start_tick == 0 && gameplay_scene.crash_tick == 0)
            {
                gameplay_scene.is_driving = true;
                gameplay_scene.start_tick = logic_layer.curr_tick;
            }
            break;
        
        default:
            process_global_keyboard(event_key);
    }
    return;
}


static void _process_pause_keyboard(const SDL_Keycode event_key)
{
    graphics_layer.screen_changed = true;

    /// Pause screen button handling:
    switch(event_key)
    {
        case SDLK_RETURN:
            if (gameplay_scene.pause_screen.close_button.is_focused || gameplay_scene.pause_screen.continue_button.is_focused)
            {
                hide_pause_screen(&gameplay_scene.pause_screen);
                ma_sound_start(&audio_manager.music);
            }
            else if (gameplay_scene.pause_screen.quit_to_menu_button.is_focused)
                logic_layer.remain_in_scene = false;
            else if (gameplay_scene.pause_screen.quit_to_desktop_button.is_focused)
                logic_layer.game_is_running = false;
            else
                graphics_layer.screen_changed = false;
            break;
        
        case SDLK_UP:
            if (get_neighbor(gameplay_scene.pause_screen.curr_focus, UP) == NULL)
            {
                graphics_layer.screen_changed = false;
                break;
            }
            unfocus(gameplay_scene.pause_screen.curr_focus);
            focus(get_neighbor(gameplay_scene.pause_screen.curr_focus, UP));
            gameplay_scene.pause_screen.curr_focus = get_neighbor(gameplay_scene.pause_screen.curr_focus, UP);
            break;
        
        case SDLK_DOWN:
            if (get_neighbor(gameplay_scene.pause_screen.curr_focus, DOWN) == NULL)
            {
                graphics_layer.screen_changed = false;
                break;
            }
            unfocus(gameplay_scene.pause_screen.curr_focus);
            focus(get_neighbor(gameplay_scene.pause_screen.curr_focus, DOWN));
            gameplay_scene.pause_screen.curr_focus = get_neighbor(gameplay_scene.pause_screen.curr_focus, DOWN);
            break;
        
        default:
            process_global_keyboard(event_key);
            graphics_layer.screen_changed = false;
            break;
    }

    return;
}


void _process_gameplay_car_input(void)
{   
    /// Input reading
    const bool* key_state = SDL_GetKeyboardState(NULL); /// Unlike the scancodes, this provides no delay - just like handling a car requires.
    gameplay_scene.car_ptr->direction_x = - key_state[SDL_SCANCODE_LEFT] + key_state[SDL_SCANCODE_RIGHT];
    /// Boundary processing
    if (gameplay_scene.car_ptr->direction_x != 0)
        gameplay_scene.car_ptr->prev_turn_direction_x = gameplay_scene.car_ptr->direction_x;
    if (gameplay_scene.car_ptr->direction_x == -1 && gameplay_scene.car_ptr->coords.x <= 0)
        gameplay_scene.car_ptr->direction_x = 0;
    else if (gameplay_scene.car_ptr->direction_x == 1 && gameplay_scene.car_ptr->coords.x + gameplay_scene.car_ptr->coords.w >= RENDER_WIDTH)
        gameplay_scene.car_ptr->direction_x = 0;
    
    /// Turn smoothing
    if (gameplay_scene.car_ptr->direction_x != 0 && gameplay_scene.car_ptr->latest_turn_start == 0)
    {
        gameplay_scene.car_ptr->latest_turn_start = logic_layer.curr_tick;
        gameplay_scene.car_ptr->latest_turn_end   = 0;
    }
    if (gameplay_scene.car_ptr->direction_x == 0 &gameplay_scene.car_ptr->latest_turn_end == 0)
    {
        gameplay_scene.car_ptr->latest_turn_end   = logic_layer.curr_tick;
        gameplay_scene.car_ptr->latest_turn_start = 0;
    }

    gameplay_scene.car_ptr->base_texture = (unsigned short)(2 + gameplay_scene.car_ptr->direction_x);
    /// NOTE: those nested `if-else`s are for overflow sanitizer compliance.
    /// - If turn recently started.
    if ((gameplay_scene.car_ptr->latest_turn_start != 0) && (logic_layer.curr_tick - gameplay_scene.car_ptr->latest_turn_start >= gameplay_scene.car_ptr->turn_smoothing_duration))
    {
        if (gameplay_scene.car_ptr->prev_turn_direction_x > 0)
            gameplay_scene.car_ptr->base_texture += (unsigned short)gameplay_scene.car_ptr->prev_turn_direction_x;
        else
        {
            if (gameplay_scene.car_ptr->base_texture <  (unsigned short)(-gameplay_scene.car_ptr->prev_turn_direction_x))
                gameplay_scene.car_ptr->base_texture = 0;
            else
                gameplay_scene.car_ptr->base_texture -= (unsigned short)(-gameplay_scene.car_ptr->prev_turn_direction_x);
        }
    }
    
    /// - If turn recently ended.
    else if ((gameplay_scene.car_ptr->latest_turn_end   != 0) && (logic_layer.curr_tick - gameplay_scene.car_ptr->latest_turn_end   <= gameplay_scene.car_ptr->turn_smoothing_duration))
    {
        if (gameplay_scene.car_ptr->prev_turn_direction_x > 0)
            gameplay_scene.car_ptr->base_texture += (unsigned short)gameplay_scene.car_ptr->prev_turn_direction_x;
        else
        {
            if (gameplay_scene.car_ptr->base_texture <  (unsigned short)(-gameplay_scene.car_ptr->prev_turn_direction_x))
                gameplay_scene.car_ptr->base_texture = 0;
            else
                gameplay_scene.car_ptr->base_texture -= (unsigned short)(-gameplay_scene.car_ptr->prev_turn_direction_x);
        }
    }
    
    /// Turning the texture
    int coord_based_diff = 0;
    if (gameplay_scene.car_ptr->coords.x <= 30)
        coord_based_diff =  2;
    else if (gameplay_scene.car_ptr->coords.x <= 60)
        coord_based_diff =  1;
    else if (gameplay_scene.car_ptr->coords.x + gameplay_scene.car_ptr->coords.w >= 210)
        coord_based_diff = -2;
    else if (gameplay_scene.car_ptr->coords.x + gameplay_scene.car_ptr->coords.w >= 180)
        coord_based_diff = -1;
    
    /// NOTE: again, integer overflow compliance.
    if (coord_based_diff > 0)
    {
        gameplay_scene.car_ptr->base_texture += (unsigned short)coord_based_diff;
        if (gameplay_scene.car_ptr->base_texture >= 5)
            gameplay_scene.car_ptr->base_texture = 4;
    }
    else
    {
        if (gameplay_scene.car_ptr->base_texture <  (unsigned short)(-coord_based_diff))
            gameplay_scene.car_ptr->base_texture = 0;
        else
            gameplay_scene.car_ptr->base_texture -= (unsigned short)(-coord_based_diff);
    }
    
    /// Moving the car across the screen.
    gameplay_scene.car_ptr->coords.x += (float)(gameplay_scene.car_ptr->direction_x) * (float)gameplay_scene.car_ptr->handling * (float)(FPS_manager.delta_ns) / SEC_IN_NS;
    return;
}