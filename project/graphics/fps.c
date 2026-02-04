/* Related header */
#include "fps.h"

/* Headers */
#include <stdbool.h>  /// `bool fps_capped`.
#include <stdio.h>    /// I/O.
#include <stdlib.h>   /// `UINT_MAX`.
#include "../debug.h" /// Error printing.
#include "../helpers/helpers.h" /// Colored output.


struct FPS_Manager FPS_manager = {0};


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