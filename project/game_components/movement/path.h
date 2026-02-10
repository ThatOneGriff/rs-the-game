#pragma once
#ifndef PATH_H
#define PATH_H

/* Headers */
#include <SDL3/SDL.h>  /// `SDL_FRect()`.
//#include <stdbool.h> /// `bool is_loop` (For now, every path is a loop).


/* Struct */

struct Path
{
    SDL_FRect* points;
    size_t     pt_count;
    //bool is_loop; /// For now, every path is a loop.
};


/* Predef */

struct Path  new_path(const SDL_FRect *const points, const size_t pt_count, int *const exit_code);
void        free_path(struct Path *const target);


#endif /// PATH_H