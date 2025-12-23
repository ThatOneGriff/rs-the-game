#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <SDL3/SDL.h> /// `SDL_FRect`
#include <stdlib.h>   /// `calloc()` / `realloc()`
#include "../debug.h" /// Error printing.


/// Structs */

struct Vec2
{
    int x;
    int y;
};

/*struct Path
{
    SDL_FRect* coords;
    size_t count;
    size_t _max_count;
}


/// Predef /

struct Path new_path(const size_t initial_count, int* exit_code);
void        del_path(struct Path* target);
void     add_to_path(struct Path* to, const SDL_FRect new_coords, int* exit_code);


/// Functions /

struct Path new_path(const size_t initial_count, int* exit_code)
{
    struct Path result;
    result.coords = NULL;
    result.count = 0;
    result.max_count = initial_count;

    /// Param checking /
    if (exit_code == NULL)
        print_warning("`new_path()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (initial_count == 0)
        print_warning("`new_path()`: `initial_count` arg is 0. Do you really want that?", NON_SDL_ERROR);
    
    /// Path creation /
    result.coords = calloc(initial_count, sizeof(SDL_FRect));
    if (result.coords == NULL)
    {
        print_error("`new_path()`: couldn't allocate memory to coordinates", NON_SDL_ERROR);
        result.max_count = 0;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    *exit_code = EXIT_SUCCESS;
    return result;
}


void del_path(struct Path* target)
{
    if (target == NULL)
        return;
    
    if (target->coords != NULL)
    {
        free(target->coords);
        target->coords = NULL;
    }

    target->count = 0;
    target->_max_count = 0;
    return;
}


void add_to_path(struct Path* to, const SDL_FRect new_coords, int* exit_code)
{
    /// Param checking /
    if (exit_code == NULL)
        print_warning("`add_to_path()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (to == NULL)
    {
        print_error("`add_to_path()`: `to` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (to->coords == NULL)
    {
        print_error("`add_to_path()`: `to->coords` is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Realloc if needed /
    if (to->_max_count <= to->count)
    {
        void* temp = NULL;
        if (to->max_count * 1.5 == to->max_count) /// i.e. minimal memory allocation is 1.
            temp = realloc(to->coords, (to->max_count += 1));
        else
            temp = realloc(to->coords, (to->max_count *= 1.5));
        
        if (temp == NULL)
        {
            print_error("`add_to_path()`: couldn't reallocate more memory", NON_SDL_ERROR);
            *exit_code = EXIT_FAILURE;
            return;
        }
        else
            to->coords = temp;
    }

    /// Insertion /
    to->coords[to->count++] = new_coords;
}
*/

#endif /// GEOMETRY_H