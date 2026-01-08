#pragma once
#ifndef PATH_H
#define PATH_H

/* SDL3 */
#include <SDL3/SDL.h>                   /// `SDL_DestroyTexture()`.
#include <SDL3_image/SDL_image.h>       /// `IMG_Load()`.

/* C headers */
#include <stdlib.h>                     /// `*alloc()`.

/* Helpers */
#include "../debug.h"                   /// Error printing.
#include "../resources.h"               /// Null texture.
#include "../graphics/graphics_layer.h" /// `graphics_layer`.
#include "../logic/logic_layer.h"       /// `logic_layer.curr_tick`.


/* Struct */

/* For now, my idea is: */
/// - `path_pts` - points of the path. They stay constant besides `init_path` and `free_path()`.
/// `struct Path` is a possible member of `game_components` members.
///  If they do get coupled to a path (via a function, maybe?), they move along the path before rendering.
struct Path
{
    SDL_FRect* path_pts;
    size_t     pt_count;
};


/* Predef */

struct Path init_path(const size_t pt_count, int* exit_code);
void        free_path(struct Path* target);

//void add_to_multi_texture(struct Multi_Texture* to, const SDL_FRect new_rects, int* exit_code);
//void render_multi_texture(const struct Multi_Texture* target);


/* Body */

struct Path init_path(const size_t max_count, int* exit_code)
{
    struct Path result;
    result.path_pts  = NULL;
    result.max_count = 0; /// Temporary value to be changed once memory is successfully allocated.

    /// Param checking
    if (exit_code == NULL)
        print_warning("`init_path()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (max_count == 0) /// Maybe it's an error at this, non-memory-dynamic, point?
        print_warning("`init_path()`: `max_count` arg is 0. Do you really want that?", NON_SDL_ERROR);

    /// Object creation
    result.path_pts  = calloc(max_count, sizeof(SDL_FRect));
    if (result.path_pts == NULL)
    {
        print_error("`init_path()`: couldn't allocate memory to path points", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.max_count = max_count;
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_path(struct Path* target)
{
    if (target == NULL)
        return;
    
    if (target->path_pts != NULL)
    {
        free(result.path_pts);
        result.path_pts = NULL;
    }

    target->cur_count = 0;
    target->max_count = 0;
}


/* Functions */

void add_to_multi_texture(struct Multi_Texture* to, const SDL_FRect new_rect, int* exit_code)
{
    /// Param checking
    if (exit_code == NULL)
        print_warning("`add_to_multi_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (to == NULL)
    {
        print_error("`add_to_multi_texture()`: `to` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (to->rects == NULL)
    {
        print_error("`add_to_multi_texture()`: `to->rects` is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Check if full
    if (to->max_count == to->count)
    {
        print_error("`add_to_multi_texture()`: `to->rects` is full", NON_SDL_ERROR);
        return;
    }

    /// Insertion
    to->rects[to->count++] = new_rect;
}


void render_multi_texture(const struct Multi_Texture* target)
{
    if (target == NULL || target->texture == NULL || target->rects == NULL || target->count == 0)
    {
        print_error("`render_multi_texture()`: `target` or its members are invalid", NON_SDL_ERROR);
        return;
    }
    
    for (size_t i = 0; i < target->count; i++)
    {
        if (target->rects[i].x + target->rects[i].w <= 0 ||
            target->rects[i].y + target->rects[i].h <= 0 ||
            target->rects[i].x >= RENDER_WIDTH ||
            target->rects[i].y >= RENDER_HEIGHT)
        {
            print_warning("`render_multi_texture()`: texture rendering out of bounds", NON_SDL_ERROR);
            continue; /// While it's not an error, why waste a draw call on something not seen anyway?
        }
        else
            SDL_RenderTexture(graphics_layer.renderer, target->texture, NULL, &target->rects[i]);
    }
}

#endif /// PATH_H