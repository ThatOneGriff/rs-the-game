#pragma once
#ifndef MULTI_TEXTURE_H
#define MULTI_TEXTURE_H

#include <SDL3/SDL.h>             /// `SDL_DestroyTexture()`.
#include <SDL3_image/SDL_image.h> /// `IMG_Load()`.

#include <stdlib.h> /// `calloc()`, `realloc()`.

#include "../graphics/graphics_layer.h" /// `graphics_layer`.


/// NOTE: NOT AT ALL RELATED to `texture.h`:
///  this presents a raw `SDL_Texture` with an array of coordinates.


/* Struct */

struct Multi_Texture
{
    SDL_Texture* texture;
    SDL_FRect*   coords;
    size_t count;
    size_t max_count;
};


/* Predef */

struct Multi_Texture new_multi_texture(const char* texture_name, const size_t initial_count, int* exit_code);
void                 del_multi_texture(struct Multi_Texture* target);

void add_to_multi_texture(struct Multi_Texture* to, const SDL_FRect new_coords, int* exit_code);
void render_multi_texture(const struct Multi_Texture* target);


/* Body */

struct Multi_Texture new_multi_texture(const char* texture_name, const size_t initial_count, int* exit_code)
{
    struct Multi_Texture result;
    result.coords = NULL;
    result.count = 0;
    result.max_count = 0;

    /* Param checking */
    if (exit_code == NULL)
        print_warning("`new_multi_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (initial_count == 0)
        print_warning("`new_multi_texture()`: `initial_count` arg is 0. Do you really want that?", NON_SDL_ERROR);
    
    if (texture_name == NULL)
    {
        print_error("`new_multi_texture()`: `texture_path` arg is `NULL`", NON_SDL_ERROR);
        result.texture = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /* Object creation */
    result.texture = IMG_LoadTexture(graphics_layer.renderer, texture_name);
    if (result.texture == NULL)
    {
        print_error("`new_multi_texture()`: couldn't load texture", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.max_count = initial_count;
    result.coords = calloc(initial_count, sizeof(SDL_FRect));
    if (result.coords == NULL)
    {
        print_error("`new_multi_texture()`: couldn't allocate memory to coordinates", NON_SDL_ERROR);
        SDL_DestroyTexture(result.texture);
        result.texture = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void del_multi_texture(struct Multi_Texture* target)
{
    if (target == NULL)
        return;
    
    if (target->texture != NULL)
    {
        SDL_DestroyTexture(target->texture);
        target->texture = NULL;
    }
    
    if (target->coords != NULL)
    {
        free(target->coords);
        target->coords = NULL;
    }

    target->count = 0;
    target->max_count = 0;
}


/* Functions */

void add_to_multi_texture(struct Multi_Texture* to, const SDL_FRect new_coords, int* exit_code)
{
    /* Param checking */
    if (exit_code == NULL)
        print_warning("`add_to_multi_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (to == NULL)
    {
        print_error("`add_to_multi_texture()`: `to` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (to->coords == NULL)
    {
        print_error("`add_to_multi_texture()`: `to->coords` is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /* Realloc if needed */
    if (to->max_count <= to->count)
    {
        void* temp = NULL;
        if (to->max_count * 1.5 == to->max_count) /// i.e. minimal memory allocation is 1.
            temp = realloc(to->coords, to->max_count + 1);
        else
            temp = realloc(to->coords, to->max_count * 1.5);
        
        if (temp == NULL)
        {
            print_error("`add_to_multi_texture()`: couldn't reallocate more memory", NON_SDL_ERROR);
            *exit_code = EXIT_FAILURE;
            return;
        }
        else
            to->coords = temp;
    }

    /* Insertion */
    to->coords[to->count++] = new_coords;
}


void render_multi_texture(const struct Multi_Texture* target)
{
    if (target == NULL || target->texture == NULL || target->coords == NULL || target->count == 0)
    {
        print_error("`render_multi_texture()`: `target` or its members are invalid", NON_SDL_ERROR);
        return;
    }
    
    for (size_t i = 0; i < target->count; i++)
        SDL_RenderTexture(graphics_layer.renderer, target->texture, NULL, &target->coords[i]);
}


#endif /// MULTI_TEXTURE_H