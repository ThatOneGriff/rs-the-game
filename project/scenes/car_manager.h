#pragma once
#ifndef CAR_MANAGER_H
#define CAR_MANAGER_H

#include <stdlib.h>   /// `*alloc()`.
#include "car.h"      /// Cars.
#include "../debug.h" /// Error message printing.

#define CAR_PATHS_PATH "./rsdt/car_data/_car-paths.rsdt"


/* Struct */

struct Car_Manager
{
    struct Car* cars;
    size_t car_count;
    size_t cur_car;
};
static struct Car_Manager car_manager; /// Singleton.


/* Predef */

void init_car_manager(int* exit_code);
void free_car_manager(void);
struct Car* select_curr_car(void);
struct Car* select_next_car(void);
struct Car* select_prev_car(void);


/* Body */

void init_car_manager(int* exit_code)
{
    /// REDO: memory-unsafe in case of failures.
    if (exit_code == NULL)
        print_warning("`init_car_manager()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Object preparation
    car_manager.cars = NULL;
    car_manager.car_count = 0; /// Temporary value to be changed once the memory is allocated.
    car_manager.cur_car    = 0;

    /// File opening
    FILE* car_data_file = fopen(CAR_PATHS_PATH, "r");
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
    car_manager.cars = malloc(line_count * sizeof(struct Car));
    if (car_manager.cars == NULL)
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
        car_manager.cars[i] = load_car(line, exit_code);
        if (*exit_code == EXIT_FAILURE)
        {
            print_error("`init_car_manager()`: couldn't load a car", NON_SDL_ERROR);
            fclose(car_data_file);
            return;
        }
    }

    car_manager.car_count = line_count;
    fclose(car_data_file);
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_car_manager(void)
{
    if (car_manager.cars != NULL)
    {
        for (size_t i = 0; i < car_manager.car_count; i++)
            free_car(&car_manager.cars[i]);
        free(car_manager.cars);
        car_manager.cars = NULL;
    }

    car_manager.car_count = 0;
    car_manager.cur_car   = 0;
    return;
}


/// TODO: checks.
struct Car* select_curr_car(void)
{
    return &car_manager.cars[car_manager.cur_car];
}


/// TODO: checks.
struct Car* select_next_car(void)
{
    ++car_manager.cur_car;
    if (car_manager.cur_car == car_manager.car_count)
        car_manager.cur_car  = car_manager.car_count - 1;
    return &car_manager.cars[car_manager.cur_car];
}


/// TODO: checks.
struct Car* select_prev_car(void)
{
    if (car_manager.cur_car > 0)
        --car_manager.cur_car;
    return &car_manager.cars[car_manager.cur_car];
}

#endif /// CAR_MANAGER_H