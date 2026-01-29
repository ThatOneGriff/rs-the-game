#pragma once
#ifndef GAMEPLAY_SCENE_H
#define GAMEPLAY_SCENE_H

/* SDL3 */
#include <SDL3/SDL.h> /// SDL3.

/* Graphics & components */
#include "../../logic/logic_layer.h"                         /// `time_tick_ms`.
#include "../car.h"                                          /// `*car_ptr` (i.e. player).
#include "pause_screen.h"                                    /// Pause screen.
#include "traffic_manager.h"                                 /// Traffic manager.
#include "../../game_components/environment.h"               /// `struct Environment`.
#include "../../game_components/graphics/multi_texture.h"    /// Multi-textures.
#include "../../game_components/graphics/shifting_texture.h" /// Shifting textures.
#include "../../game_components/graphics/texture.h"          /// Textures.
#include "../../game_components/movement/move_component.h"   /// `struct Move_Component`.

#define GAMEPLAY_DATA_LINES 20
#define CLOUD_VELOCITY 150


/* Struct */

struct Gameplay_Scene
{
    bool is_driving;
    time_tick_ms start_tick;
    time_tick_ms crash_tick;
    int point_count;
    int prev_point_count;

    SDL_Texture* sky_bg; /// Not a `Texture`, because it's rendered on the whole screen.
    struct Shifting_Texture ground;
    struct Shifting_Texture road;
    struct Shifting_Texture stripes;
    struct Environment      trees;

    struct Texture clouds[10];
    struct Texture personal_best_text;
    struct Texture   curr_points_text;

    struct Car* car_ptr;
    struct Pause_Screen pause_screen;
};


/* Predef */

struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code);
void                  free_gameplay_scene(struct Gameplay_Scene* target);
void                  render_gameplay_scene(struct Gameplay_Scene* target);
void update_points(struct Gameplay_Scene* target);

#endif /// GAMEPLAY_SCENE_H