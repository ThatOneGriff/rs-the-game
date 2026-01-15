#pragma once
#ifndef MENU_EVENTS_H
#define MENU_EVENTS_H

/* SDL3 */
#include <SDL3/SDL.h> /// Keyboard processing.

/* Logic */
#include <stdbool.h>                   /// Bools.
#include "../../logic/global_events.h" /// Global event processing.
#include "../../logic/logic_layer.h"   /// Key state variable.

/* Scene & components */
#include "menu_scene.h"                   /// Menu scene manipulation.
#include "options_screen.h"               /// Options screen manipulation.
#include "../../audio/audio_manager.h"    /// Audio
#include "../../audio/music_loader.h"     ///  manipulation.
#include "../../game_components/button.h" /// Button manipulation.
#include "../../game_components/switch.h" /// Switch manipulation.
#include "../../graphics/fps.h"           /// Frame cap control.

/// NOTE: not my proudest code.
/// I would've shortened a lot of things, if not for the time constraints.


/* Predef */

void  process_menu_events  (struct Menu_Scene* scene);
void _process_menu_keyboard(struct Menu_Scene* scene, const SDL_Keycode event_key);


/* Body */

void process_menu_events(struct Menu_Scene* scene)
{
    while (SDL_PollEvent(&logic_layer.event))
    {
        switch (logic_layer.event.type)
        {
        /// Key press.
        case SDL_EVENT_KEY_DOWN:
            _process_menu_keyboard(scene, logic_layer.event.key.key);
            break;
        /// Other event.
        default:
            process_global_events(logic_layer.event);
        }
    }
}


void _process_menu_keyboard(struct Menu_Scene* scene, const SDL_Keycode event_key)
{
    switch(event_key)
    {
        case SDLK_RETURN:
        if (! scene->options_screen.is_open)
        {
            if (scene->play_button.is_focused)
                logic_layer.remain_in_scene = false;
            else if (scene->options_button.is_focused)
                show_options_screen(&scene->options_screen);
            else if (scene->quit_button.is_focused)
                logic_layer.game_is_running = false;
            break;
        }
        else if (scene->options_screen.is_open)
        {
            if (scene->options_screen.audio_switch.is_focused)
            {
                audio_manager.using_audio = ! audio_manager.using_audio;
                change_switch_option(&scene->options_screen.audio_switch);
                if (! audio_manager.using_audio)
                    ma_sound_stop(&audio_manager.music);
                else
                    play_random_music(&music_loader_menu);
            }
            else if (scene->options_screen.close_button.is_focused)
            {
                hide_options_screen(&scene->options_screen);
            }
            else if (scene->options_screen.fps_switch.is_focused)
            {       
                ++curr_fps_cap_i;
                if (curr_fps_cap_i == 4)
                    curr_fps_cap_i = 0;
                set_fps_cap(fps_cap_options[curr_fps_cap_i]);
                change_switch_option(&scene->options_screen.fps_switch);
            }
            break;
        }

        /// TEMP while I'm coming up with a better button management structure.
        case SDLK_UP:
        if (! scene->options_screen.is_open)
        {
            if (scene->options_button.is_focused)
            {
                scene->play_button.   is_focused = true;
                scene->options_button.is_focused = false;
            }
            else if (scene->quit_button.is_focused)
            {
                scene->options_button.is_focused = true;
                scene->quit_button.   is_focused = false;
            }
            break;
        }
        else if (scene->options_screen.is_open)
        {
            if (scene->options_screen.audio_switch.is_focused)
            {
                scene->options_screen.audio_switch.is_focused = false;
                scene->options_screen.close_button.is_focused = true;
            }
            else if (scene->options_screen.fps_switch.is_focused)
            {
                scene->options_screen.fps_switch.  is_focused = false;
                scene->options_screen.audio_switch.is_focused = true;
            }
            break;
        }
        
        case SDLK_DOWN:
        if (! scene->options_screen.is_open)
        {
            if (scene->play_button.is_focused)
            {
                scene->play_button.   is_focused = false;
                scene->options_button.is_focused = true;
            }
            else if (scene->options_button.is_focused)
            {
                scene->options_button.is_focused = false;
                scene->quit_button.   is_focused = true;
            }
            break;
        }
        else if (scene->options_screen.is_open)
        {
            if (scene->options_screen.close_button.is_focused)
            {
                scene->options_screen.close_button.is_focused = false;
                scene->options_screen.audio_switch.is_focused = true;
            }
            else if (scene->options_screen.audio_switch.is_focused)
            {
                scene->options_screen.audio_switch.is_focused = false;
                scene->options_screen.fps_switch.is_focused   = true;
            }
            break;
        }
        
        case SDLK_M: /// TEMP: will be extended to playing next/previous track and pausing.
            play_random_music(&music_loader_menu);
            break;

        default:
            _process_global_keyboard(event_key);
    }
}

#endif /// MENU_EVENTS_H