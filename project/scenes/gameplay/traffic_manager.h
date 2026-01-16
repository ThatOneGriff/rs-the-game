#pragma once
#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

/* Helpers */
#include "../../debug.h"             /// Error message printing.
#include "../../deinit_stack.h"      /// Deinitialization stack.
#include "../../helpers/geometry.h"  /// Path flipping & collision checking.
#include "../../helpers/random.h"    /// Random.
#include "../../logic/logic_layer.h" /// `logic_layer.curr_tick`.

/* Related headers */
#include "../car.h"         /// Cars.
#include "../car_manager.h" /// `traffic_car_manager`.
#include "../../game_components/movement/path.h" /// Traffic path.

#define TRAFFIC_ON_ROW_CHANCE     (float)0.50 /// percent
#define TRAFFIC_ON_2_LANES_CHANCE (float)0.70 /// percent

#define LANE_SPAWN_COOLDOWN 5


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
    /// TODO: ptr to a function (to check collisions with traffic).
};
static struct Traffic_Manager traffic_manager;


/* Predef */

void init_traffic_manager(const size_t car_count, int* exit_code);
void free_traffic_manager(void);

void   move_traffic       (void);
void render_traffic_on_pts(const size_t min_path_pt, size_t max_path_pt, struct Car* player_car, bool* is_driving);


/* Body */

/// Assumes `traffic_car_manager` has been initialized.
void init_traffic_manager(const size_t car_count, int* exit_code)
{
    /// Object preparation
    traffic_manager.cars      = NULL;
    traffic_manager.car_count = 0; /// Temporary value to be changed once memory is successfully allocated.
    traffic_manager.car_path_pts = NULL;
    traffic_manager.car_lane_ids = NULL;
    traffic_manager.latest_move_tick = 0; /// Will be set with the first move.
    traffic_manager.move_delta       = 150;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`init_traffic_manager()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (car_count == 0)
    {
        print_error("`init_traffic_manager()`: `car_count` arg = 0", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(5 + car_count, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_traffic_manager()`: couldn't create deinit stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Cars (memory)
    traffic_manager.cars = malloc(car_count * sizeof(struct Car));
    if (traffic_manager.cars == NULL)
    {
        print_error("`init_traffic_manager()`: couldn't allocate memory for cars", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, traffic_manager.cars, (void (*)(void*))free);

    /// Car path points
    traffic_manager.car_path_pts = malloc(car_count * sizeof(size_t));
    if (traffic_manager.car_path_pts == NULL)
    {
        print_error("`init_traffic_manager()`: couldn't allocate memory for car path points", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    for (size_t i = 0; i < traffic_manager.car_count; i++)
        traffic_manager.car_path_pts[i] = ULONG_LONG_MAX; /// Means the car is not in traffic.
    add_to_deinit_stack(&deinit_stack, traffic_manager.car_path_pts, (void (*)(void*))free);

    /// Car lane IDs
    traffic_manager.car_lane_ids = malloc(car_count * sizeof(size_t));
    if (traffic_manager.car_lane_ids == NULL)
    {
        print_error("`init_traffic_manager()`: couldn't allocate memory for car lane IDs", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    for (size_t i = 0; i < traffic_manager.car_count; i++)
        traffic_manager.car_lane_ids[i] = ULONG_LONG_MAX; /// Means the car is not on any lane.
    add_to_deinit_stack(&deinit_stack, traffic_manager.car_lane_ids, (void (*)(void*))free);

    /// Cars (getting randomized)
    for (size_t i = 0; i < car_count; i++)
    {
        traffic_manager.cars[i] = copy_random_car(&traffic_car_manager);
        add_to_deinit_stack(&deinit_stack, &traffic_manager.cars[i], (void (*)(void*))free_car);
    }
    traffic_manager.car_count = car_count;

    /// Path (left lane)
    traffic_manager.lanes[0] = new_path(
        (SDL_FRect[]){{ 95, 75,10,10},  {85, 70,15,15}, { 77, 68,20,20},
                      { 70, 70,25,25},  {63, 73,29,29}, { 50, 75,35,35},
                      { 35, 80,45,45},  {15, 85,55,55}, {-10, 90,65,65},
                      {-20,100,65,65}, {-35,120,75,75}, {-50,140,85,85}},
                       12, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_traffic_manager()`: error creating left lane path", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, &traffic_manager.lanes[0], (void (*)(void*))free_path);
    
    /// Path (center lane)
    traffic_manager.lanes[1] = new_path(
        (SDL_FRect[]){{center_x(10), 75,10,10}, {center_x(15), 70,15,15}, {center_x(20), 68,20,20},
                      {center_x(25), 70,25,25}, {center_x(29), 73,29,29}, {center_x(35), 75,35,35},
                      {center_x(45), 80,45,45}, {center_x(55), 85,55,55}, {center_x(65), 90,65,65},
                      {center_x(65),100,65,65}, {center_x(75),120,75,75}, {center_x(85),140,85,85}},
                       12, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_traffic_manager()`: error creating center lane path", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, &traffic_manager.lanes[1], (void (*)(void*))free_path);
    
    /// Path (right lane)
    traffic_manager.lanes[2] = flipped_path_x(traffic_manager.lanes[0]);

    traffic_manager.lane_spawn_cooldowns[0] = 0;
    traffic_manager.lane_spawn_cooldowns[1] = 0;
    traffic_manager.lane_spawn_cooldowns[2] = 0;

    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_traffic_manager(void)
{
    if (traffic_manager.cars != NULL)
    {
        for (size_t i = 0; i < traffic_manager.car_count; i++)
            free_car(&traffic_manager.cars[i]);
        free(traffic_manager.cars);
        traffic_manager.cars = NULL;
    }
    if (traffic_manager.car_path_pts != NULL)
    {
        free(traffic_manager.car_path_pts);
        traffic_manager.car_path_pts = NULL;
    }
    if (traffic_manager.car_lane_ids != NULL)
    {
        free(traffic_manager.car_lane_ids);
        traffic_manager.car_lane_ids = NULL;
    }
    traffic_manager.car_count = 0;

    traffic_manager.latest_move_tick = 0;
    traffic_manager.move_delta       = 0;

    traffic_manager.lane_spawn_cooldowns[0] = 0;
    traffic_manager.lane_spawn_cooldowns[1] = 0;
    traffic_manager.lane_spawn_cooldowns[2] = 0;

    free_path(&traffic_manager.lanes[0]);
    free_path(&traffic_manager.lanes[1]);
    free_path(&traffic_manager.lanes[2]);
    return;
}


void move_traffic(void)
{
    /// Delta checking
    if (traffic_manager.latest_move_tick == 0)
    {
        traffic_manager.latest_move_tick = logic_layer.curr_tick;
        return;
    }
    if (logic_layer.curr_tick - traffic_manager.latest_move_tick < traffic_manager.move_delta)
        return;
    
    if   (traffic_manager.lane_spawn_cooldowns[0] > 0)
        --traffic_manager.lane_spawn_cooldowns[0];
    if   (traffic_manager.lane_spawn_cooldowns[1] > 0)
        --traffic_manager.lane_spawn_cooldowns[1];
    if   (traffic_manager.lane_spawn_cooldowns[2] > 0)
        --traffic_manager.lane_spawn_cooldowns[2];
    
    /// Moving traffic
    for (size_t car_id = 0; car_id < traffic_manager.car_count; car_id++)
    {
        if (traffic_manager.car_lane_ids[car_id] == ULONG_LONG_MAX) /// Non-active traffic car
            continue;
        ++traffic_manager.  car_path_pts[car_id];
        
        /// Removing car
        if (traffic_manager.car_path_pts[car_id] >= traffic_manager.lanes[0].pt_count)
        {
            traffic_manager.car_lane_ids[car_id] = ULONG_LONG_MAX;
            traffic_manager.car_path_pts[car_id] = ULONG_LONG_MAX;
        }

        traffic_manager.latest_move_tick = logic_layer.curr_tick;
    }

    if (traffic_manager.lane_spawn_cooldowns[0] > 0
     && traffic_manager.lane_spawn_cooldowns[1] > 0
     && traffic_manager.lane_spawn_cooldowns[2] > 0) /// No free lanes
        return;
    
    /// Seeding traffic on row
    //if (rand_percent(0, 100) <= TRAFFIC_ON_ROW_CHANCE)
    
    for (size_t i = 0; i < 2; i++)
    {
        const size_t lane = (size_t)randint(0,2);
        if (traffic_manager.lane_spawn_cooldowns[lane] > 0)
            return;

        /// Checking for car availability
        size_t car_id = ULONG_LONG_MAX;
        for (size_t j = 0; j < traffic_manager.car_count; j++)
            if (traffic_manager.car_lane_ids[j] == ULONG_LONG_MAX)
                car_id = j;
        if (car_id == ULONG_LONG_MAX) /// All cars taken.
            break;
        
        /// Setting car on a lane
        traffic_manager.car_lane_ids[car_id] = lane;
        traffic_manager.car_path_pts[car_id] = 0;

        traffic_manager.cars[car_id].base_texture = 4 - lane * 2;
        traffic_manager.lane_spawn_cooldowns[lane] = LANE_SPAWN_COOLDOWN;

        if (i == 1 && rand_percent(0,100) <= TRAFFIC_ON_2_LANES_CHANCE)
            continue;
        traffic_manager.lane_spawn_cooldowns[0] = LANE_SPAWN_COOLDOWN;
        traffic_manager.lane_spawn_cooldowns[1] = LANE_SPAWN_COOLDOWN;
        traffic_manager.lane_spawn_cooldowns[2] = LANE_SPAWN_COOLDOWN;
        break;
    }
    return;
}


void render_traffic_on_pts(const size_t min_path_pt, size_t max_path_pt, struct Car* player_car, bool* is_driving)
{
    /// TODO: check args
    if (max_path_pt >= traffic_manager.lanes[0].pt_count)
        max_path_pt  = traffic_manager.lanes[0].pt_count - 1;
    
    for (size_t path_pt = min_path_pt; path_pt <= max_path_pt; path_pt++)
    {
        for (size_t car_id = 0; car_id < traffic_manager.car_count; car_id++)
        {
            if (traffic_manager.car_path_pts[car_id] == path_pt)
            {
                const size_t lane_id = traffic_manager.car_lane_ids[car_id];
                traffic_manager.cars[car_id].coords = traffic_manager.lanes[lane_id].points[path_pt];
                render_car(&traffic_manager.cars[car_id]);

                if (path_pt == 9) /// Collision check
                {
                    SDL_FRect players_collision_box = CAR_COLLISION_BOXES[traffic_manager.cars[car_id].base_texture];
                    players_collision_box.x -= (RENDER_WIDTH -                  player_car->coords.x);
                    SDL_FRect traffic_collision_box = CAR_COLLISION_BOXES[traffic_manager.cars[car_id].base_texture];
                    traffic_collision_box.x -= (RENDER_WIDTH - traffic_manager.cars[car_id].coords.x);
                    if (have_x_overlap(players_collision_box, traffic_collision_box))
                    {
                        UNUSED(is_driving); /// TEMP
                        //*is_driving = false;
                        printf("collision %s %s\n", player_car->name, traffic_manager.cars[car_id].name); /// TEMP
                    }
                }
            }
        }
    }

    return;
}

#endif /// TRAFFIC_MANAGER_H