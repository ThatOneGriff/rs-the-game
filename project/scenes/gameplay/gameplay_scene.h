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
#include "../../game_components/graphics/multi_sprite.h"    /// Multi-textures.
#include "../../game_components/graphics/animated_sprite.h" /// Shifting textures.
#include "../../game_components/graphics/sprite.h"          /// Textures.
#include "../../game_components/movement/move_component.h"   /// `struct Move_Component`.

#define GAMEPLAY_DATA_LINES 20
#define CLOUD_VELOCITY 150


/* Struct */

struct Gameplay_Scene
{
    bool is_driving;
    time_tick_ms start_tick;
    time_tick_ms crash_tick;
    unsigned point_count;
    unsigned prev_point_count;

    SDL_Texture* sky_bg; /// Not a `Texture`, because it's rendered on the whole screen.
    struct Animated_Sprite ground;
    struct Animated_Sprite road;
    struct Animated_Sprite stripes;
    struct Environment      trees;

    struct Sprite clouds[10];
    struct Sprite personal_best_text;
    struct Sprite   curr_points_text;

    struct Sprite fps_text;

    struct Car* car_ptr;
    struct Pause_Screen pause_screen;
};
extern struct Gameplay_Scene gameplay_scene; /// Singleton.


/* Predef */

void load_gameplay_scene(const char *const path, struct Car *const car_ptr, int *const exit_code);
void free_gameplay_scene(void);
void render_gameplay_scene(void);
void update_fps_text(void);
void update_points  (void);


#endif /// GAMEPLAY_SCENE_H