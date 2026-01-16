#pragma once
#ifndef CAR_MANAGER_H
#define CAR_MANAGER_H

#include <stdlib.h>            /// `*alloc()`.
#include <stdbool.h>           /// `bool is_traffic`.
#include "car.h"               /// Cars.
#include "../debug.h"          /// Error message printing.
#include "../helpers/random.h" /// Random.

#define PLAYERS_CAR_DATA_PATH "./rsdt/car_data/_car-paths.rsdt"
#define TRAFFIC_CAR_DATA_PATH "./rsdt/traffic_data/_traffic-paths.rsdt"

#define LOAD_PLAYERS false
#define LOAD_TRAFFIC true



/* Struct */

struct Car_Manager
{
    struct Car* cars;
    size_t car_count;
    size_t cur_car;
};
static struct Car_Manager players_car_manager;
static struct Car_Manager traffic_car_manager;


/* Predef */

void init_car_manager(struct Car_Manager* target, const bool is_traffic, int* exit_code);
void free_car_manager(struct Car_Manager* target);
struct Car copy_random_car(struct Car_Manager* target);
struct Car* get_curr_car  (struct Car_Manager* target);
struct Car* get_next_car  (struct Car_Manager* target);
struct Car* get_prev_car  (struct Car_Manager* target);


/* Body */

void init_car_manager(struct Car_Manager* target, const bool is_traffic, int* exit_code)
{
    /// REDO: memory-unsafe in case of failures.
    if (exit_code == NULL)
        print_warning("`init_car_manager()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Objects preparation
    target->cars = NULL;
    target->car_count = 0; /// Temporary value to be changed once the memory is allocated.
    target->cur_car   = 0;

    /// File opening
    FILE* car_data_file = NULL;
    if (! is_traffic)
        car_data_file = fopen(PLAYERS_CAR_DATA_PATH, "r");
    else
        car_data_file = fopen(TRAFFIC_CAR_DATA_PATH, "r");
    if (car_data_file == NULL)
    {
        print_error("`init_car_manager()`: couldnt't open car data file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// First line indicates track amount.
    char line[100];
    fgets(line, 100, car_data_file);
    const size_t line_count = atoi(line);
    if (line_count == 0)
    {
        print_error("`init_car_manager()`: car data line count = 0", NON_SDL_ERROR);
        fclose(car_data_file);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Memory allocation.
    target->cars = malloc(line_count * sizeof(struct Car));
    if (target->cars == NULL)
    {
        print_error("`init_car_manager()`: couldn't allocate memory for cars", NON_SDL_ERROR);
        fclose(car_data_file);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Car loading
    for (size_t i = 0; i < line_count; i++)
    {
        fgets(line, 100, car_data_file);
        line[strcspn(line, "\n")] = '\0';
        if (! is_traffic)
            target->cars[i] = load_car        (line, exit_code);
        else
            target->cars[i] = load_traffic_car(line, exit_code);
        if (*exit_code == EXIT_FAILURE)
        {
            print_error("`init_car_manager()`: couldn't load a car", NON_SDL_ERROR);
            fclose(car_data_file);
            return;
        }
    }

    target->car_count = line_count;
    fclose(car_data_file);
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_car_manager(struct Car_Manager* target)
{
    if (target->cars != NULL)
    {
        for (size_t i = 0; i < target->car_count; i++)
            free_car(&target->cars[i]);
        free(target->cars);
        target->cars = NULL;
    }

    target->car_count = 0;
    target->cur_car   = 0;
    return;
}


struct Car copy_random_car(struct Car_Manager* target)
{
    return target->cars[(size_t)randint(0, (unsigned)target->car_count-1)];
}


/// TODO: checks.
struct Car* get_curr_car(struct Car_Manager* target)
{
    return &target->cars[target->cur_car];
}


/// TODO: checks.
struct Car* get_next_car(struct Car_Manager* target)
{
    ++target->cur_car;
    if (target->cur_car == target->car_count)
        target->cur_car  = target->car_count - 1;
    return &target->cars[target->cur_car];
}


/// TODO: checks.
struct Car* get_prev_car(struct Car_Manager* target)
{
    if (target->cur_car > 0)
        --target->cur_car;
    return &target->cars[target->cur_car];
}

#endif /// CAR_MANAGER_H