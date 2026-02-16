#pragma once
#ifndef CAR_MANAGER_H
#define CAR_MANAGER_H

#include <stdbool.h>           /// `bool is_traffic`.
#include "car.h"               /// Cars.


/* Struct */

struct Car_Manager
{
    struct Car* cars;
    size_t car_count;
    size_t cur_car;
};
extern struct Car_Manager players_car_manager;


/* Predef */

void init_car_manager(int *const exit_code);
void free_car_manager(void);
struct Car* get_curr_car(void);
struct Car* get_next_car(void);
struct Car* get_prev_car(void);


#endif /// CAR_MANAGER_H