#pragma once
#ifndef FPS_H
#define FPS_H

/* Headers */
#include <stdbool.h>  /// `bool fps_capped`.
#include "../debug.h" /// Error printing.

#define SEC_IN_NS (float)1000000000.0
#define time_tick_ns unsigned long long int
#define time_tick_ms unsigned long long int /// Purely for code clarity.

static unsigned int fps_cap_options[5] = {15, 30, 60, 120, UINT_MAX};
static unsigned int curr_fps_cap_i = 2;


/* Struct */

struct FPS_Manager
{
    bool         fps_capped;
    unsigned int fps_cap;
    time_tick_ns        delta_ns;
    time_tick_ns target_delta_ns;
};
static struct FPS_Manager FPS_manager; /// Singleton.


/* Predef */

void set_fps_cap(const unsigned int new_fps_cap);
void rem_fps_cap(void);
void print_compare_fps(const unsigned int curr_fps, const unsigned int prev_fps);


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
        FPS_manager.target_delta_ns = (time_tick_ns)(SEC_IN_NS / (float)new_fps_cap);
    }
}


void rem_fps_cap(void)
{
    FPS_manager.fps_capped = false;
}


void print_compare_fps(const unsigned int curr_fps, const unsigned int prev_fps)
{
    if (curr_fps == prev_fps || prev_fps == UINT_MAX) /// `prev_fps == 0` means it's the first FPS measurement
    {
        textcolor(GRAY);
        printf("[~]");
        textcolor(WHITE);
    }
    else if (curr_fps > prev_fps)
    {
        textcolor(GREEN);
        printf("[+]");
        textcolor(WHITE);
    }
    else if (curr_fps < prev_fps)
    {
        textcolor(RED);
        printf("[-]");
        textcolor(WHITE);
    }
    printf(" %u FPS\n", curr_fps);
}

#endif /// FPS_H