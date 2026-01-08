#pragma once
#ifndef MOVE_COMPONENT_H
#define MOVE_COMPONENT_H

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_FRect()`.

/* C headers */
#include <stdbool.h>              /// `bool randomize_positions`
#include <stdlib.h>               /// `*alloc()`.

/* Related */
#include "path.h"                 /// The whole `Move_Component` is based on some `Path`.
#include "../logic/logic_layer.h" /// `logic_layer.curr_tick`.

/* Helpers */
#include "../debug.h"             /// Error printing.
#include "../helpers/random.h"    /// Position randomizing.


/* Struct */

struct Move_Component
{
    struct Path path;
    size_t      rect_count;
    SDL_FRect** manipulated_rects;
    size_t*     rects_pt_indices;

    time_tick_ms latest_move;
    time_tick_ms step;
};


/* Predef */

struct Move_Component new_move_component(const struct Path path,
                                         SDL_FRect** manipulated_rects, const size_t rect_count,
                                         const time_tick_ms step, bool randomize_positions, int* exit_code);
void move_all_rects     (struct Move_Component* target);
void free_move_component(struct Move_Component* target);


/* Body */

struct Move_Component new_move_component(const struct Path path,
                                         SDL_FRect** manipulated_rects, const size_t rect_count,
                                         const time_tick_ms step, bool randomize_positions, int* exit_code)
{
    struct Move_Component result;
    result.path = path;
    result.rect_count = 0; /// Temporary value to be changed once memory is successfully allocated.
    result.latest_move = 0; /// Will be filled with first move.
    result.step        = step;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`new_move_component()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (rect_count == 0)
    {
        print_error("`new_move_component()`: `rect_count` arg is 0. `Move_Component` is not dynamic-memory", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (manipulated_rects == NULL)
    {
        print_error("`new_move_component()`: `manipulated_rects` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Object creation
    result.manipulated_rects = calloc(rect_count, sizeof(SDL_FRect*));
    if (result.manipulated_rects == NULL)
    {
        print_error("`new_move_component()`: couldn't allocate memory for `manipulated_rects`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.rects_pt_indices = calloc(rect_count, sizeof(size_t));
    if (result.rects_pt_indices == NULL)
    {
        print_error("`new_move_component()`: couldn't allocate memory for `rects_pt_indices`", NON_SDL_ERROR);
        free(result.manipulated_rects);
        result.manipulated_rects = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.rect_count = rect_count;

    /// Position distribution
    if (randomize_positions)
        for (size_t i = 0; i < rect_count; i++)
            result.rects_pt_indices[i] = randint(0, (unsigned)path.pt_count-1); /// UNTESTED
    
    else if (! randomize_positions)
        for (size_t i = 0; i < rect_count; i++)
            result.rects_pt_indices[i] = i % path.pt_count; /// UNTESTED
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void move_all_rects(struct Move_Component* target)
{
    if (target == NULL)
    {
        print_error("`move_all_rects()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    if (logic_layer.curr_tick - target->latest_move < target->step)
        return;
    
    for (size_t i = 0; i < target->rect_count; i++)
    {
        ++target->rects_pt_indices[i];
        if (target->rects_pt_indices[i] >= target->path.pt_count)
        {
            /*if (target->path.is_loop)
                --target->rects_pt_indices[i];
            else*/ /// For now, every path is a loop.
            target->rects_pt_indices[i] = 0;
        }
        
        *target->manipulated_rects[i] = target->path.path_pts[target->rects_pt_indices[i]]; /// UNTESTED
    }

    return;
}


void free_move_component(struct Move_Component* target)
{
    if (target == NULL)
    {
        print_error("`free_move_component()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    free_path(&target->path);
    target->latest_move = 0;
    target->step        = 0;

    if (target->manipulated_rects != NULL)
    {
        free(target->manipulated_rects);
        target->manipulated_rects = NULL;
    }
    target->rect_count = 0;

    if (target->rects_pt_indices != NULL)
    {
        free(target->rects_pt_indices);
        target->rects_pt_indices = NULL;
    }

    return;
}

#endif /// MOVE_COMPONENT_H