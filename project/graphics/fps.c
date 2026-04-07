/* Related header */
#include "fps.h"

/* Headers */
#include <SDL3/SDL.h> /// `SDL_Delay()`.
#include <stdbool.h>  /// `bool fps_capped, show_fps`.
#include <stdlib.h>   /// `UINT_MAX`.
#include "../debug.h" /// Error printing.


/* Variables */

struct FPS_Manager FPS_manager = {0};
const unsigned int fps_cap_options[4] = {30, 60, 120, UINT_MAX};
unsigned int  curr_fps_cap_i = 1;
unsigned int  prev_fps = 0;
unsigned int  curr_fps = 0;
bool          show_fps = false;
struct Vec2 fps_counter_position = FPS_POS_2_DIGITS;


/* Predef */

void set_fps_cap(const unsigned int new_fps_cap);
void rem_fps_cap(void);


/* Body */

/// Also used as initialization.
void set_fps_cap(const unsigned int new_fps_cap)
{
    if (new_fps_cap == 0)
    {
        print_warning("`set_fps_cap()`: attempt to set an invalid frame cap of 0");
        return;
    }

    FPS_manager.fps_capped = true; /// `set_fps_cap()` can just be used to put the FPS cap back on.
    if (new_fps_cap == FPS_manager.fps_cap)
        return;
    
    FPS_manager.fps_cap = new_fps_cap;
    FPS_manager.target_delta_ns = (time_tick_ns)(SEC_IN_NS / (float)new_fps_cap);

    if (new_fps_cap <= 99)
        fps_counter_position = FPS_POS_2_DIGITS;
    else if (new_fps_cap <= 999)
        fps_counter_position = FPS_POS_3_DIGITS;
    else
        fps_counter_position = FPS_POS_UNLIMITED;
    return;
}


void rem_fps_cap(void)
{
    FPS_manager.fps_capped = false;
    return;
}


void wait_until_target_delta(void)
{
    if (! FPS_manager.fps_capped || FPS_manager.target_delta_ns <= FPS_manager.delta_ns)
        return;
    
    SDL_DelayNS(FPS_manager.target_delta_ns - FPS_manager.delta_ns);
    FPS_manager.delta_ns = FPS_manager.target_delta_ns;
    return;
}