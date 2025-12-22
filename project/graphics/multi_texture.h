#pragma once
#ifndef MULTI_TEXTURE_H
#define MULTI_TEXTURE_H

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "../graphics/graphics_layer.h"

/// REDO according to new standards.

/* Struct */

struct Multi_Texture
{
    SDL_Texture* texture;
    SDL_FRect*   coords;
    size_t count;
    size_t max_count;
};


/* Predef */

struct Multi_Texture new_multi_texture(const char* texture_name, const size_t initial_size);
void                 del_multi_texture(struct Multi_Texture* target);

void add(struct Multi_Texture* to, const SDL_FRect new_coords);
void render_multi_texture(const struct Multi_Texture* target);


/* Body */

struct Multi_Texture new_multi_texture(const char* texture_name, const size_t initial_count)
{
    struct Multi_Texture result;
    result.coords = NULL;
    result.count = 0;
    result.max_count = 0;

    if (texture_name == NULL)
    {
        fprintf(stderr, "~ [ERROR] Multi-texture initialization: no texture name given.\n");
        return result;
    }

    if (initial_count == 0)
        fprintf(stderr, "~ [WRNNG] Multi-texture initialization: initial count set to 0; do you really want that?.\n");

    result.texture = IMG_LoadTexture(graphics_layer.renderer, texture_name);
    if (result.texture == NULL)
    {
        fprintf(stderr, "~ [ERROR] Multi-texture image loading failed: %s.\n", SDL_GetError());
        return result;
    }
    result.max_count = initial_count;
    result.coords = calloc(initial_count, sizeof(SDL_FRect));
    if (result.coords == NULL)
    {
        fprintf(stderr, "~ [ERROR] Multi-texture: can't allocate memory to coordinates.\n");
        return result;
    }
    
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

void add(struct Multi_Texture* to, const SDL_FRect new_coords)
{
    if (to == NULL)
    {
        fprintf(stderr, "~ [ERROR] Trying to `add()` into an empty `Multi_Texture`.\n");
        return;
    }

    /// Enlargening the array if needed
    if (to->max_count <= to->count)
    {
        void* temp = NULL;
        if (to->max_count * 1.5 > to->max_count + 1) /// i.e. minimal enlargement is 1.
            temp = realloc(to->coords, to->max_count * 1.5);
        else
            temp = realloc(to->coords, to->max_count + 1);
        
        if (temp == NULL)
        {
            fprintf(stderr, "~ [ERROR] Failed to add memory to a `Multi_Texture`.\n");
            return;
        }

        to->coords = temp;
    }

    /// Adding data and count
    to->coords[to->count++] = new_coords;
}


void render_multi_texture(const struct Multi_Texture* target)
{
    if (target == NULL || target->texture == NULL || target->coords == NULL || target->count == 0)
        return;
    
    for (size_t i = 0; i < target->count; i++)
        SDL_RenderTexture(graphics_layer.renderer, target->texture, NULL, &target->coords[i]);
}


#endif /// MULTI_TEXTURE_H