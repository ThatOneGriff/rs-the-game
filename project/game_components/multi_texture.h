#pragma once
#ifndef MULTI_TEXTURE_H
#define MULTI_TEXTURE_H

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_DestroyTexture()`.
#include <SDL3_image/SDL_image.h> /// `IMG_Load()`.

/* C headers */
#include <stdlib.h>               /// `*alloc()`.

/* Related headers */
#include "move_component.h"       /// Moving textures.

/* Helpers */
#include "../debug.h"                   /// Error printing.
#include "../resources.h"               /// Null texture.
#include "../graphics/graphics_layer.h" /// `graphics_layer`.


/// NOTE: NOT AT ALL RELATED to `texture.h`:
///  this presents a raw `SDL_Texture` with an array of coordinates.


/* Struct */

struct Multi_Texture
{
    SDL_Texture* texture;
    SDL_FRect*   rects;
    size_t cur_count;
    size_t max_count;

    struct Move_Component* move_component;
};


/* Predef */

struct Multi_Texture     load_multi_texture(const char* texture_path, const size_t max_count, int* exit_code);
void couple_move_component_to_multi_texture(struct Multi_Texture* to, struct Move_Component* move_component, bool randomize_positions, int* exit_code);
void                     free_multi_texture(struct Multi_Texture* target);

void add_to_multi_texture(struct Multi_Texture* to, const SDL_FRect new_rects, int* exit_code);
void render_multi_texture(const struct Multi_Texture* target);


/* Body */

struct Multi_Texture load_multi_texture(const char* texture_path, const size_t max_count, int* exit_code)
{
    struct Multi_Texture result;
    result.rects = NULL;
    result.cur_count = 0;
    result.max_count = 0;
    result.move_component = NULL;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`new_multi_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (max_count == 0)
        print_warning("`new_multi_texture()`: `max_count` arg is 0. Do you really want that?", NON_SDL_ERROR);
    
    if (texture_path == NULL)
    {
        print_error("`new_multi_texture()`: `texture_path` arg is `NULL`", NON_SDL_ERROR);
        result.texture = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Object creation
    result.texture = IMG_LoadTexture(graphics_layer.renderer, texture_path);
    if (result.texture == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`new_multi_texture()`: couldn't load the texture, replaced with null texture", IS_SDL_ERROR);
            result.texture = NULL_TEXTURE;
        }
        else
        {
            print_error("`new_multi_texture()`: couldn't load the texture, and null texture is empty", IS_SDL_ERROR);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }
    result.rects = calloc(max_count, sizeof(SDL_FRect));
    if (result.rects == NULL)
    {
        print_error("`new_multi_texture()`: couldn't allocate memory to coordinates", NON_SDL_ERROR);
        SDL_DestroyTexture(result.texture);
        result.texture = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.max_count = max_count;
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void couple_move_component_to_multi_texture(struct Multi_Texture* to, struct Move_Component* move_component, bool randomize_positions, int* exit_code)
{
    /// Arg checking
    if (exit_code == NULL)
        print_warning("`couple_move_component_to_multi_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (to == NULL)
    {
        print_error("`couple_move_component_to_multi_texture()`: `to` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (move_component == NULL)
    {
        print_error("`couple_move_component_to_multi_texture()`: `move_component` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    couple_move_component(move_component, &to->rects, to->cur_count, randomize_positions, exit_code); /// UNTESTED
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`couple_move_component_to_multi_texture()`: coupling failed", NON_SDL_ERROR);
        return;
    }

    *exit_code = EXIT_SUCCESS;
    return;
}


void free_multi_texture(struct Multi_Texture* target)
{
    if (target == NULL)
        return;
    
    if (target->texture != NULL && target->texture != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->texture);
        target->texture = NULL;
    }
    
    if (target->rects != NULL)
    {
        free(target->rects);
        target->rects = NULL;
    }

    if (target->move_component != NULL)
    {
        free_move_component(target->move_component);
        target->move_component = NULL;
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
    if (to->max_count == to->cur_count)
    {
        print_error("`add_to_multi_texture()`: `to->rects` is full", NON_SDL_ERROR);
        return;
    }

    /// Insertion
    to->rects[to->cur_count++] = new_rect;
}


void render_multi_texture(const struct Multi_Texture* target)
{
    if (target == NULL || target->texture == NULL || target->rects == NULL || target->cur_count == 0)
    {
        print_error("`render_multi_texture()`: `target` or its members are invalid", NON_SDL_ERROR);
        return;
    }

    if (target->move_component != NULL)
        move_all_rects(target->move_component); /// UNTESTED
    
    for (size_t i = 0; i < target->cur_count; i++)
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

#endif /// MULTI_TEXTURE_H