#pragma once
#ifndef MENU_EVENTS_H
#define MENU_EVENTS_H

/* SDL3 */
#include <SDL3/SDL.h> /// Keyboard processing.

/* Logic */
#include "../../logic/global_events.h" /// Global event processing.
#include "../../logic/logic_layer.h"   /// Key state variable.

/* Scene & components */
#include "menu_scene.h"                     /// Menu scene manipulation.
#include "../../game_components/button.h"   /// Button manipulation (maybe TEMP?).


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
            if (scene->play_button.is_focused)
                logic_layer.remain_in_scene = false;
            break;

        /// TEMP while I'm coming up with a better button management structure.
        case SDLK_UP:
        case SDLK_DOWN:
            scene->dummy_button.is_focused = ! scene->dummy_button.is_focused;
            scene->play_button.is_focused  = ! scene->play_button.is_focused;
            break;
        
        case SDLK_M: /// TEMP: will be extended to playing next/previous track and pausing.
            play_random_music(&music_loader_menu);
            break;

        default:
            _process_global_keyboard(event_key);
    }
}

#endif /// MENU_EVENTS_H