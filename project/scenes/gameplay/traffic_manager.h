#pragma once
#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

/* Helpers */
#include "../../logic/logic_layer.h" /// `time_tick_ms`.

/* Related headers */
#include "../car.h" /// Cars.
#include "../../game_components/movement/path.h" /// Traffic's path.

#define TRAFFIC_ON_ROW_CHANCE     0.50f /// (percent)
#define TRAFFIC_ON_2_LANES_CHANCE 0.70f /// (percent)

#define LANE_SPAWN_COOLDOWN 5

#define MOVE_NORMAL false
#define MOVE_REVERSE true


/* Struct */

struct Traffic_Manager
{
    struct Car* cars;
    size_t      car_count;
    size_t*  car_lane_ids;
    size_t*  car_path_pts;

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