#pragma once
#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

/* Helpers */
#include "../../debug.h"          /// Error message printing.
#include "../../deinit_stack.h"   /// Deinitialization stack.
#include "../../helpers/random.h" /// Random.

/* Related headers */
#include "../car.h"         /// Cars.
#include "../car_manager.h" /// `traffic_car_manager`.
#include "../../game_components/movement/path.h" /// Traffic path.


/* Struct */

struct Traffic_Manager
{
    struct Car* cars;
    size_t      car_count;

    struct Path   left_lane;
    struct Path center_lane;
    struct Path  right_lane;

    /// TODO: ptr to a function (to check collisions with traffic).
};
static struct Traffic_Manager traffic_manager;


/* Predef */

void init_traffic_manager(const size_t car_count, int* exit_code);
void free_traffic_manager(void);

void        render_traffic(const struct Traffic_Manager* target);
void partly_render_traffic(const struct Traffic_Manager* target, const size_t min_path_pt, size_t max_path_pt);


/* Body */

/// Assumes `traffic_car_manager` has been initialized.
void init_traffic_manager(const size_t car_count, int* exit_code)
{
    /// Object preparation
    traffic_manager.cars      = NULL;
    traffic_manager.car_count = 0; /// Temporary value to be changed once memory is successfully allocated.

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
    struct Deinit_Stack deinit_stack = new_deinit_stack(4 + car_count, exit_code);
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
        (SDL_FRect[]){{80,75, 7, 7},  {75,70,10,10},  {65,65,15,15},
                      {55,60,22,22},  {35,60,30,30},
                      {5,55,45,45}, {-30,50,60,60}, {-60,45,75,75}},
                       8, exit_code);
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
        (SDL_FRect[]){{80,75, 7, 7},  {75,70,10,10},  {65,65,15,15},
                      {55,60,22,22},  {35,60,30,30},
                      {5,55,45,45}, {-30,50,60,60}, {-60,45,75,75}},
                       8, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_traffic_manager()`: error creating center lane path", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, &traffic_manager.center_lane, (void (*)(void*))free_path);
    
    /// Path (right lane)
    traffic_manager.right_lane = new_path(
        (SDL_FRect[]){{80,75, 7, 7},  {75,70,10,10},  {65,65,15,15},
                      {55,60,22,22},  {35,60,30,30},
                      {5,55,45,45}, {-30,50,60,60}, {-60,45,75,75}},
                       8, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_traffic_manager()`: error creating right lane path", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, &traffic_manager.right_lane, (void (*)(void*))free_path);

    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return;
}

#endif /// TRAFFIC_MANAGER_H