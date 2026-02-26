/* Related header */
#include "fps.h"

/* Headers */
#include <stdbool.h>  /// `bool fps_capped, show_fps`.
#include <stdio.h>    /// I/O.
#include <stdlib.h>   /// `UINT_MAX`.
#include "../debug.h" /// Error printing.


/* Variables */

struct FPS_Manager FPS_manager = {0};
const unsigned int fps_cap_options[4] = {30, 60, 120, UINT_MAX};
unsigned int  curr_fps_cap_i = 1;
unsigned int  prev_fps = 0;
unsigned int  curr_fps = 0;
bool          show_fps = false;


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
        FPS_manager.target_delta_ns = (time_tick_ns)(SEC_IN_NS / (float)new_fps_cap);
    }
}


void rem_fps_cap(void)
{
    FPS_manager.fps_capped = false;
}