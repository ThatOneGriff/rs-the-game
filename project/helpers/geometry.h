#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

/* Headers */
#include "../debug.h"                         /// Error message printing.
#include "../game_components/movement/path.h" /// Path manipulation.
#include "../graphics/graphics_layer.h"       /// `RENDER_WIDTH/HEIGHT`.


/* Structs */

struct Vec2
{
    float x;
    float y;
};
#define X_AUTO_CENTER -1
#define Y_AUTO_CENTER -1


/* Predef */

/// REDO: rewrite all using macros.

struct Path flipped_path_x(const struct Path path);
float center_x  (const float w);
float center_y  (const float h);
float reflect_x (const float x);
float reflect_y (const float y);
struct Vec2 vec2(const float x, const float y);


/* Body */

struct Path flipped_path_x(const struct Path path) /// UNTESTED
{
    struct Path result;
    result.points = malloc(path.pt_count * sizeof(SDL_FRect));
    if (result.points == NULL)
    {
        print_error("`flipped_path_x()`: couldn't allocate memory", NON_SDL_ERROR);
        return result; /// TODO: add `exit_code`.
    }
    for (size_t i = 0; i < path.pt_count; i++)
    {
        result.points[i] = path.points[i];
        result.points[i].x = reflect_x(path.points[i].x) - path.points[i].w;
    }
    result.pt_count = path.pt_count;
    return result;
}

float center_x(const float w)
{
    return (RENDER_WIDTH - w) / (float)2.0;
}

float center_y(const float h)
{
    return (RENDER_HEIGHT - h) / (float)2.0;
}

float reflect_x(const float x)
{
    return RENDER_WIDTH - x;
}

float reflect_y(const float y)
{
    return RENDER_HEIGHT - y;
}

struct Vec2 vec2(const float x, const float y)
{
    struct Vec2 result;
    result.x = x;
    result.y = y;
    return result;
}

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