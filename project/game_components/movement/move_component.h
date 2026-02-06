#pragma once
#ifndef MOVE_COMPONENT_H
#define MOVE_COMPONENT_H

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_FRect()`.

/* C headers */
#include <stdbool.h>  /// `bool randomize_positions`

/* Related */
#include "path.h"     /// The whole `Move_Component` is based on some `Path`.

/* Helpers */
#include "../../logic/logic_layer.h" /// `time_tick_ms`.
#include "../../helpers/geometry.h"  /// `struct Vec2`.

#define RANDOMIZED_POSITIONS true
#define    ORDERED_POSITIONS false


/* Struct */

struct Move_Component
{
    struct Path path;
    time_tick_ms latest_move;
    time_span_ms step;

    bool  random_x_reflect; /// If `true`, the coordinates will randomly reflect along X axis (like trees do).
    bool* reflected_rect_indices;

    SDL_FRect**  manipulated_rects;
    size_t       rect_count;
    struct Vec2* offsets;
    struct Vec2  max_offset;
    size_t*      rects_pt_indices;
};


/* Predef */

struct Move_Component init_move_component(const struct Path path, const time_span_ms step, bool random_x_reflect, int* exit_code);
void                couple_move_component(struct Move_Component* target, SDL_FRect* manipulated_rects, const size_t rect_count,
                                          struct Vec2 max_offset, bool randomize_positions, int* exit_code);
void move_all_rects     (struct Move_Component* target);
void free_move_component(struct Move_Component* target);


#endif /// MOVE_COMPONENT_H