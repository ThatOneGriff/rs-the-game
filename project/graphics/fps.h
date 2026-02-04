#pragma once
#ifndef FPS_H
#define FPS_H

/* Headers */
#include <stdbool.h> /// `bool fps_capped`.
#include <stdlib.h>  /// `UINT_MAX`.
#include "../logic/logic_layer.h" /// `time_tick_ns`.


#define SEC_IN_NS (float)1000000000.0

/// Purely for code clarity.
#define time_tick_ns unsigned long long int
#define time_tick_ms unsigned long long int
#define time_span_ns unsigned long long int
#define time_span_ms unsigned long long int

static unsigned int fps_cap_options[4] = {30, 60, 120, UINT_MAX};
static unsigned int curr_fps_cap_i = 1;


/* Struct */

struct FPS_Manager
{
    bool         fps_capped;
    unsigned int fps_cap;
    time_tick_ns        delta_ns;
    time_tick_ns target_delta_ns;
};
extern struct FPS_Manager FPS_manager; /// Singleton.


/* Predef */

void set_fps_cap(const unsigned int new_fps_cap);
void rem_fps_cap(void);
void print_compare_fps(const unsigned int curr_fps, const unsigned int prev_fps);


#endif /// FPS_H