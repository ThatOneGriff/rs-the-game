#ifndef FPS_H
#define FPS_H

/* Headers */
#include <stdbool.h>              /// `bool fps_capped`.
#include <stdlib.h>               /// `UINT_MAX`.
#include "../helpers/geometry.h"  /// `struct Vec2`.
#include "../logic/logic_layer.h" /// `time_tick_ns`.

#define SEC_IN_NS 1000000000.0f
extern const unsigned int fps_cap_options[4];
extern unsigned int  curr_fps_cap_i;
extern unsigned int  prev_fps;
extern unsigned int  curr_fps;
extern bool          show_fps;

#define FPS_POS_2_DIGITS  vec2(165, 5)
#define FPS_POS_3_DIGITS  vec2(160, 5)
#define FPS_POS_UNLIMITED vec2(155, 5)
extern struct Vec2 fps_counter_position;


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


#endif /// FPS_H