/* Related header */
#include "car_manager.h"

#include <stdbool.h>           /// `bool is_traffic`.
#include <stdlib.h>            /// `*alloc()`.
#include <stdio.h>             /// File read/write.
#include <string.h>            /// `memset()`.
#include "car.h"               /// Cars.
#include "../debug.h"          /// Error message printing.
#include "../helpers/random.h" /// Random.

#define PLAYERS_CAR_DATA_PATH "./rsdt/car_data/_car-paths.rsdt"


/* Variables */

struct Car_Manager players_car_manager = {0};


/* Predef */

void init_car_manager(int *const exit_code);
void free_car_manager(void);
struct Car* get_curr_car(void);
struct Car* get_next_car(void);
struct Car* get_prev_car(void);


/* Body */

void init_car_manager(int *const exit_code)
{
    /// REDO: memory-unsafe in case of failures.
    if (exit_code == NULL)
        print_warning("`init_car_manager()`: `exit_code` arg is `NULL`");

    /// File opening
    FILE* car_data_file = fopen(PLAYERS_CAR_DATA_PATH, "r");
    if (car_data_file == NULL)
    {
        print_error("`init_car_manager()`: couldnt't open car data file");
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// First line indicates track amount.
    char line[100];
    fgets(line, 100, car_data_file);
    const size_t line_count = (size_t)atoi(line);
    if (line_count == 0)
    {
        print_error("`init_car_manager()`: car data line count = 0");
        fclose(car_data_file);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Memory allocation.
    players_car_manager.cars = malloc(line_count * sizeof(struct Car));
    if (players_car_manager.cars == NULL)
    {
        print_error("`init_car_manager()`: couldn't allocate memory for cars");
        fclose(car_data_file);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Car loading
    for (size_t i = 0; i < line_count; i++)
    {
        fgets(line, 100, car_data_file);
        line[strcspn(line, "\n")] = '\0';
        players_car_manager.cars[i] = load_car(line, exit_code);
        if (*exit_code == EXIT_FAILURE)
        {
            print_error("`init_car_manager()`: couldn't load a car");
            fclose(car_data_file);
            return;
        }
    }

    players_car_manager.car_count = line_count;
    fclose(car_data_file);
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_car_manager(void)
{
    if (players_car_manager.cars != NULL)
    {
        for (size_t i = 0; i < players_car_manager.car_count; i++)
            free_car(&players_car_manager.cars[i]);
        free(players_car_manager.cars);
    }

    memset(&players_car_manager, 0, sizeof players_car_manager);
    return;
}


/// TODO: checks.
struct Car* get_curr_car(void)
{
    return &players_car_manager.cars[players_car_manager.cur_car];
}


/// TODO: checks.
struct Car* get_next_car(void)
{
    ++players_car_manager.cur_car;
    if (players_car_manager.cur_car == players_car_manager.car_count)
        players_car_manager.cur_car  = players_car_manager.car_count - 1;
    return &players_car_manager.cars[players_car_manager.cur_car];
}


/// TODO: checks.
struct Car* get_prev_car(void)
{
    if (players_car_manager.cur_car > 0)
        --players_car_manager.cur_car;
    return &players_car_manager.cars[players_car_manager.cur_car];
}