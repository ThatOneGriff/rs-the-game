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
#include "../deinit_stack.h"      /// Deinitialization stack.
#include "../helpers/geometry.h"  /// `struct Vec2`, `reflect_x`.
#include "../helpers/random.h"    /// Position randomizing.

#define RANDOMIZED_POSITIONS true
#define    ORDERED_POSITIONS false


/* === IN CASE YOU END UP HERE ===
I wholeheartedly hate this code. Sorry. */


/* Struct */

struct Move_Component
{
    struct Path path;
    time_tick_ms latest_move;
    time_tick_ms step;

    struct Vec2 max_jitter; /// Jitter along X and Y axis.
    bool  random_x_reflect; /// If `true`, the coordinates will randomly reflect along X axis (like trees do).
    bool* reflected_rect_indices;

    SDL_FRect**  manipulated_rects;
    size_t       rect_count;
    struct Vec2* offsets;
    struct Vec2  max_offset;
    size_t*      rects_pt_indices;
};


/* Predef */

struct Move_Component init_move_component(const struct Path path, const time_tick_ms step, bool random_x_reflect, int* exit_code);
void                couple_move_component(struct Move_Component* target, SDL_FRect* manipulated_rects, const size_t rect_count, struct Vec2 max_offset, bool randomize_positions, int* exit_code);
void move_all_rects     (struct Move_Component* target);
void free_move_component(struct Move_Component* target);


/* Body */

struct Move_Component init_move_component(const struct Path path, const time_tick_ms step, bool random_x_reflect, int* exit_code)
{
    struct Move_Component result;
    result.path = path;
    result.rect_count = 0; /// Temporary value to be changed once memory is successfully allocated.
    result.latest_move = 0; /// Will be filled with first move.
    result.step        = step;
    result.random_x_reflect = random_x_reflect;
    
    result.reflected_rect_indices = NULL;
    result.manipulated_rects      = NULL;
    result.offsets                = NULL;
    result.rects_pt_indices       = NULL;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`new_move_component()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void couple_move_component(struct Move_Component* target, SDL_FRect* manipulated_rects, const size_t rect_count, struct Vec2 max_offset, bool randomize_positions, int* exit_code)
{
    /// Param checking
    if (exit_code == NULL)
        print_warning("`couple_move_component()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (target == NULL)
    {
        print_error("`couple_move_component()`: `target` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (rect_count == 0)
    {
        print_error("`couple_move_component()`: `rect_count` arg is 0. `Move_Component` is not dynamic-memory", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (manipulated_rects == NULL)
    {
        print_error("`couple_move_component()`: `manipulated_rects` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(3, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`new_environment()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return;
    }

    /// Coupling
    target->manipulated_rects = calloc(rect_count, sizeof(SDL_FRect*));
    if (target->manipulated_rects == NULL)
    {
        print_error("`couple_move_component()`: couldn't allocate memory for `manipulated_rects`", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    for (size_t i = 0; i < rect_count; i++)
        target->manipulated_rects[i] = &manipulated_rects[i];
    add_to_deinit_stack(&deinit_stack, target->manipulated_rects, (void (*)(void*))free);
    
    target->max_offset.x = max_offset.x;
    target->max_offset.y = max_offset.y;
    if (! (max_offset.x == 0 && max_offset.y == 0)) /// Non-zero offset.
    {
        target->offsets = calloc(rect_count, sizeof(struct Vec2));
        if (target->manipulated_rects == NULL)
        {
            print_error("`couple_move_component()`: couldn't allocate memory for `manipulated_rects`", NON_SDL_ERROR);
            flush_deinit_stack(&deinit_stack);
            *exit_code = EXIT_FAILURE;
            return;
        }

        for (size_t i = 0; i < rect_count; i++)
            target->offsets[i] = rand_vec2(vec2(-max_offset.x, -max_offset.y), vec2(0,0));

        add_to_deinit_stack(&deinit_stack, target->offsets, (void (*)(void*))free);
    }

    target->rects_pt_indices = calloc(rect_count, sizeof(size_t));
    if (target->rects_pt_indices == NULL)
    {
        print_error("`couple_move_component()`: couldn't allocate memory for `rects_pt_indices`", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, target->rects_pt_indices, (void (*)(void*))free);

    target->rect_count = rect_count;
    if (target->random_x_reflect)
    {
        target->reflected_rect_indices = calloc(target->rect_count, sizeof(bool)); /// NOTE: could we pack that into an `int`?
        if (target->reflected_rect_indices == NULL)
        {
            print_error("`couple_move_component()`: couldn't allocate memory for `reflected_rect_indices`", NON_SDL_ERROR);
            flush_deinit_stack(&deinit_stack);
            *exit_code = EXIT_FAILURE;
            return;
        }
        //add_to_deinit_stack(&deinit_stack, target->random_x_reflect, (void (*)(void*))free);
    }

    /// Position distribution (+ setting rects to the path's first point)
    for (size_t i = 0; i < rect_count; i++)
    {
        if (randomize_positions)
            target->rects_pt_indices[i] = randint(0, (unsigned)target->path.pt_count-1);
        else
            target->rects_pt_indices[i] = i % target->path.pt_count;
        *(target->manipulated_rects[i]) = target->path.points[target->rects_pt_indices[i]];
        
        if (target->random_x_reflect)
            target->reflected_rect_indices[i] = randint(0,1);
    }
    
    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return;
}


void move_all_rects(struct Move_Component* target)
{
    if (target == NULL)
    {
        print_error("`move_all_rects()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }
    if (target->manipulated_rects == NULL || target->rects_pt_indices == NULL || target->rect_count == 0)
    {
        print_error("`move_all_rects()`: trying to use an uncoupled `target`", NON_SDL_ERROR);
        return;
    }

    /// Tick check
    if (target->latest_move == 0)
    {
        target->latest_move = logic_layer.curr_tick; /// Only triggered upon 1st move.
        return;
    }
    else if (logic_layer.curr_tick - target->latest_move < target->step)
        return;
    
    /// Moving all manipulated coords
    for (size_t i = 0; i < target->rect_count; i++)
    {
        ++target->rects_pt_indices[i];
        if (target->rects_pt_indices[i] >= target->path.pt_count)
        {
            /// For now, every path is a loop.

            /*if (target->path.is_loop)
                --target->rects_pt_indices[i];
            else*/

            target->rects_pt_indices[i] = 0;
            if (target->random_x_reflect)
                target->reflected_rect_indices[i] = randint(0,1);
            if (target->offsets != NULL)
                target->offsets[i] = rand_vec2(vec2(-target->max_offset.x, -target->max_offset.y), vec2(0,0));
        }
        
        SDL_FRect new_rect = target->path.points[target->rects_pt_indices[i]];
        if (target->offsets != NULL)
        {
            new_rect.x += target->offsets[i].x;
            new_rect.y += target->offsets[i].y;
        }
        if (target->random_x_reflect)
            if (target->reflected_rect_indices[i] == 1)
                new_rect.x = reflect_x(new_rect.x) - new_rect.w;
            
        *(target->manipulated_rects[i]) = new_rect;
    }

    target->latest_move = logic_layer.curr_tick;
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