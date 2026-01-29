#pragma once
#ifndef GLOBAL_EVENTS_H
#define GLOBAL_EVENTS_H

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_Event`, `SDL_Keycode`.


/* Predef */

void  process_global_events  (const SDL_Event   event);
void _process_global_keyboard(const SDL_Keycode event_key);

#endif /// GLOBAL_EVENTS_H