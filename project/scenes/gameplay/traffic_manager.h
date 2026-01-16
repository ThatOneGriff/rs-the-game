#pragma once
#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

/* Helpers */
#include "../../debug.h"             /// Error message printing.
#include "../../deinit_stack.h"      /// Deinitialization stack.
#include "../../helpers/geometry.h"  /// Path flipping.
#include "../../helpers/random.h"    /// Random.
#include "../../logic/logic_layer.h" /// `logic_layer.curr_tick`.

/* Related headers */
#include "../car.h"         /// Cars.
#include "../car_manager.h" /// `traffic_car_manager`.
#include "../../game_components/movement/path.h" /// Traffic path.

#define TRAFFIC_ON_ROW_CHANCE     20 /// percent
#define TRAFFIC_ON_2_LANES_CHANCE 60 /// percent


/* Struct */

struct Traffic_Manager
{
    struct Car* cars;
    size_t      car_count;

    struct Path   left_lane;
    struct Path center_lane;
    struct Path  right_lane;

    time_tick_ms latest_move_tick;
    time_span_ms move_delta;
    /// TODO: ptr to a function (to check collisions with traffic).
};
static struct Traffic_Manager traffic_manager;


/* Predef */

void init_traffic_manager(const size_t car_count, int* exit_code);
void free_traffic_manager(void);

void          move_traffic(void);
void        render_traffic(void);
void partly_render_traffic(const size_t min_path_pt, size_t max_path_pt);


/* Body */

/// Assumes `traffic_car_manager` has been initialized.
void init_traffic_manager(const size_t car_count, int* exit_code)
{
    /// Object preparation
    traffic_manager.cars      = NULL;
    traffic_manager.car_count = 0; /// Temporary value to be changed once memory is successfully allocated.
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
    struct Deinit_Stack deinit_stack = new_deinit_stack(3 + car_count, exit_code);
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

    /// Cars (getting randomized)
    for (size_t i = 0; i < car_count; i++)
    {
        traffic_manager.cars[i] = copy_random_car(&traffic_car_manager);
        add_to_deinit_stack(&deinit_stack, &traffic_manager.cars[i], (void (*)(void*))free_car);
    }

    /// Path (left lane)
    traffic_manager.left_lane = new_path(
        (SDL_FRect[]){{95, 75, 10,10}, {85,70,15,15}, { 77,68,20,20},
                      {70, 70, 25,25}, {63,73,29,29}, { 50,75,35,35},
                      {35, 80, 45,45}, {15,85,55,55}, {-10,90,65,65},
                      {-15,125,65,65}/*, {-25,150,65,65}*/},
                       10, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_traffic_manager()`: error creating left lane path", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, &traffic_manager.left_lane, (void (*)(void*))free_path);
    
    /// Path (center lane)
    traffic_manager.center_lane = new_path(
        (SDL_FRect[]){{center_x(10), 75, 10,10}, {center_x(15),70,15,15}, {center_x(20),68,20,20},
                      {center_x(25), 70, 25,25}, {center_x(29),73,29,29}, {center_x(35),75,35,35},
                      {center_x(45), 80, 45,45}, {center_x(55),85,55,55}, {center_x(65),90,65,65},
                      {center_x(65),125,65,65}/*, {-25,150,65,65}*/},
                       10, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_traffic_manager()`: error creating center lane path", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, &traffic_manager.center_lane, (void (*)(void*))free_path);
    
    /// Path (right lane)
    traffic_manager.right_lane = flipped_x(traffic_manager.left_lane);

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
    traffic_manager.car_count = 0;

    traffic_manager.latest_move_tick = 0;
    traffic_manager.move_delta       = 0;

    free_path(&traffic_manager.left_lane);
    free_path(&traffic_manager.center_lane);
    free_path(&traffic_manager.right_lane);
    return;
}


void move_traffic(void)
{
    if (rand_percent(0, 100) > TRAFFIC_ON_ROW_CHANCE)
        return;
    
    
}

#endif /// TRAFFIC_MANAGER_H