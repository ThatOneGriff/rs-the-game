#pragma once
#ifndef PATH_H
#define PATH_H

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_FRect()`.

/* C headers */
//#include <stdbool.h>  /// `bool is_loop` /// For now, every path is a loop.
#include <stdlib.h>   /// `*alloc()`.

/* Helpers */
#include "../debug.h" /// Error printing.


/* Struct */

struct Path
{
    SDL_FRect* path_pts;
    size_t     pt_count;
    //bool is_loop; /// For now, every path is a loop.
};


/* Predef */

struct Path init_path(const SDL_FRect* path_pts, const size_t pt_count, int* exit_code);
void        free_path(struct Path* target);


/* Body */

struct Path init_path(const SDL_FRect* path_pts, const size_t pt_count, int* exit_code)
{
    struct Path result;
    result.path_pts = NULL;
    result.pt_count = 0; /// Temporary value to be changed once memory is successfully allocated.

    /// Param checking
    if (exit_code == NULL)
        print_warning("`init_path()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (pt_count == 0)
    {
        print_error("`init_path()`: `pt_count` arg is 0. `Path` is not dynamic-memory", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (path_pts == NULL)
    {
        print_error("`init_path()`: `path_pts` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Object creation
    result.path_pts = calloc(pt_count, sizeof(SDL_FRect));
    if (result.path_pts == NULL)
    {
        print_error("`init_path()`: couldn't allocate memory to path points", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    for (size_t i = 0; i < pt_count; i++)
        result.path_pts[i] = path_pts[i];
    result.pt_count = pt_count;
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_path(struct Path* target)
{
    if (target == NULL)
        return;
    
    if (target->path_pts != NULL)
    {
        free(target->path_pts);
        target->path_pts = NULL;
    }

    target->pt_count = 0;
}

#endif /// PATH_H