
#ifndef CAR_H
#define CAR_H

/* Headers */
#include <SDL3/SDL.h>             /// SDL things.
#include "../logic/logic_layer.h" /// `time_tick_ms`.


#define CAR_DATA_LINES 16
const extern SDL_FRect CAR_COLLISION_BOXES[5]; /// TODO: think on where to move it.


/* Struct */

struct Car
{
    char name[25];
    int  year;
    int  hp;
    int  top_speed;
    int  handling;
    char quad_paths[4][64];
    char info_text [2][50];

    SDL_Texture* textures[5];
    size_t       base_texture;

    SDL_FRect     coords;
    int           direction_x;
    int prev_turn_direction_x; /// Saves the latest non-default `direction_x`. Needed for turn smoothing.

    time_tick_ms latest_turn_start;
    time_tick_ms latest_turn_end;
    time_span_ms turn_smoothing_duration;
    ///time_tick_ms latest_jump_tick;
};


/* Predef */

struct Car load_car(const char *const path, int *const exit_code);
void       free_car(struct Car *const target);
void     render_car(struct Car *const target);


#endif /// CAR_H