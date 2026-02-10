#pragma once
#ifndef CAR_H
#define CAR_H

/* Headers */
#include <SDL3/SDL.h> /// SDL things.
#include "../logic/logic_layer.h" /// `time_tick_ms`.

#define TRAFFIC_CAR_DATA_LINES 4
//const extern SDL_FRect CAR_COLLISION_BOXES[5]; /// TODO: think on where to move it.


/* Struct */

struct Traffic_Car
{
    char name[25];
    SDL_Texture* textures[3];
    size_t       base_texture;
    size_t lane_id;
    size_t path_pt;
};


/* Predef */

struct Traffic_Car load_traffic_car(const char *const path, int *const exit_code);
void               free_traffic_car(struct Traffic_Car *const target);
void             render_traffic_car(struct Traffic_Car *const target);


#endif /// CAR_H