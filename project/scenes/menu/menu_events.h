#pragma once
#ifndef MENU_EVENTS_H
#define MENU_EVENTS_H

/* Headers */
#include <SDL3/SDL.h>   /// `SDL_Keycode`.
#include "menu_scene.h" /// `struct Menu_Scene`.


/* Predef */

void  process_menu_events  (struct Menu_Scene* scene);
void _process_menu_keyboard(struct Menu_Scene* scene, const SDL_Keycode event_key);

#endif /// MENU_EVENTS_H