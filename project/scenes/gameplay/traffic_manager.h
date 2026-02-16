#pragma once
#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

/* Helpers */
#include "../../logic/logic_layer.h" /// `time_tick_ms`.

/* Related headers */
#include "../car.h"      /// Player's car manager (REDO: remove).
#include "traffic_car.h" /// Cars.
#include "../../game_components/movement/path.h" /// Traffic's path.

#define MOVE_NORMAL false
#define MOVE_REVERSE true


/* Struct */

struct Traffic_Manager
{
    struct Traffic_Car* cars;
    size_t              car_count;
    
    struct Path lanes[3];
    size_t lane_spawn_cooldowns[3];

    time_tick_ms latest_move_tick;
    time_span_ms move_delta;
};
extern struct Traffic_Manager traffic_manager; /// Singleton.


/* Predef */

void init_traffic_manager(const size_t car_count, int *const exit_code);
void free_traffic_manager(void);
void   move_traffic       (const bool mode);
void render_traffic_on_pts(const size_t min_path_pt, size_t max_path_pt, struct Car *const player_car, bool *const is_driving, int *const point_count);


#endif /// TRAFFIC_MANAGER_H