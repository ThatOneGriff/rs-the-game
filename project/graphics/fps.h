#pragma once
#ifndef FPS_H
#define FPS_H

#include "../debug.h" /// Error printing.

#define SEC_IN_NS 1000000000.0

/* Struct */

struct FPS_Manager
{
    bool fps_capped;
    unsigned int fps_cap;

    unsigned long long int target_delta_ns;
    unsigned long long int        delta_ns;
    double     fraction_of_target_delta_elapsed;
};
static struct FPS_Manager  movement_FPS_manager;
static struct FPS_Manager rendering_FPS_manager;


/* Predef */

struct FPS_Manager new_fps_manager (const unsigned int fps_cap);
void       update_both_fps_managers(const unsigned long long int delta_ns);

void set_fps_cap(const unsigned int new_value);
void rem_fps_cap();


/* Body */

struct FPS_Manager new_fps_manager(const unsigned int fps_cap)
{
    struct FPS_Manager result;
    if (fps_cap == 0)
    {
        /// An error because a broken `FPS_Manager` makes the whole rendering process UB.
        print_error("`new_fps_manager()`: Invalid frame cap of 0. Terminating FPS manager creation", NON_SDL_ERROR);
        return result;
    }

    result.fps_capped = true;
    result.fps_cap = fps_cap;
    
    result.target_delta_ns = SEC_IN_NS / fps_cap;
    result.delta_ns = result.target_delta_ns; /// For the 1st frame to render correctly.
    result.fraction_of_target_delta_elapsed = 1.0;

    return result;
}


void update_both_fps_managers(const unsigned long long int delta_ns)
{
    /* Movement */
    movement_FPS_manager.delta_ns  = delta_ns;
    movement_FPS_manager.fraction_of_target_delta_elapsed  = (double)delta_ns / movement_FPS_manager.target_delta_ns;

    /* Rendering */
    rendering_FPS_manager.delta_ns = delta_ns;
    rendering_FPS_manager.fraction_of_target_delta_elapsed = (double)delta_ns / rendering_FPS_manager.target_delta_ns;
}


void set_fps_cap(const unsigned int new_value)
{
    rendering_FPS_manager.fps_capped = true;

    if (new_value != rendering_FPS_manager.fps_cap)
    {
        rendering_FPS_manager.fps_cap = new_value;
        rendering_FPS_manager.target_delta_ns = SEC_IN_NS / (float)new_value;
    }
}


void rem_fps_cap()
{
    rendering_FPS_manager.fps_capped = false;
}

#endif /// FPS_H