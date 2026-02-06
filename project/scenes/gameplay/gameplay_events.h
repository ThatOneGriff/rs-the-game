#pragma once
#ifndef GAMEPLAY_EVENTS_H
#define GAMEPLAY_EVENTS_H

/* SDL3 */
#include <SDL3/SDL.h> /// Keyboard controls.

/* Scene & components */
#include "../car.h"         /// Car position controlling.
#include "gameplay_scene.h" /// Gameplay scene manipulation.


/* Predef */

void process_gameplay_events   (struct Gameplay_Scene* scene);
void process_gameplay_keyboard (struct Gameplay_Scene* scene, const SDL_Keycode event_key);
void process_gameplay_car_input(struct Car* car);


#endif /// GAMEPLAY_EVENTS_H