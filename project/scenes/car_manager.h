#pragma once
#ifndef CAR_MANAGER_H
#define CAR_MANAGER_H

#include <stdbool.h>           /// `bool is_traffic`.
#include "car.h"               /// Cars.

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

#endif /// CAR_MANAGER_H