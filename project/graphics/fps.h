#pragma once
#ifndef FPS_H
#define FPS_H

#include <stdbool.h>  /// `bool fps_capped`.
#include "../debug.h" /// Error printing.

#define SEC_IN_NS 1000000000.0


/* Struct */

struct FPS_Manager
{
    bool fps_capped;
    unsigned int fps_cap;
    unsigned long long int        delta_ns;
    unsigned long long int target_delta_ns;
    unsigned long long int lag_compensation_ns;
};
static struct FPS_Manager FPS_manager; /// Singleton.


/* Predef */

void set_fps_cap(const unsigned int new_fps_cap);
void rem_fps_cap(void);


/* Body */

/// Also used as initialization.
void set_fps_cap(const unsigned int new_fps_cap)
{
    if (new_fps_cap == 0)
    {
        print_warning("`set_fps_cap()`: attempt to set an invalid frame cap of 0", NON_SDL_ERROR);
        return;
    }

    FPS_manager.fps_capped = true; /// `set_fps_cap()` can just be used to put the FPS cap back on.
    if (new_fps_cap != FPS_manager.fps_cap)
    {
        FPS_manager.fps_cap = new_fps_cap;
        FPS_manager.target_delta_ns = SEC_IN_NS / new_fps_cap;
    }
}


void rem_fps_cap(void)
{
    FPS_manager.fps_capped = false;
}

#endif /// FPS_H