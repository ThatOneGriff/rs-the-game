#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* Helper headers */
#include "../debug.h"                   /// Error printing.
#include "../resources.h"               /// Null texture.
#include "../graphics/graphics_layer.h" /// Rendering of the texture.


/* Struct */

struct Texture
{
    SDL_Texture* texture;
    SDL_FRect    rect;
};
struct Texture load_texture(const char* path, const SDL_FRect rect, int* exit_code);
void           free_texture(struct Texture* target);


/* Predef */

void render_texture(const struct Texture* target);


/* Body */

struct Texture load_texture(const char* path, const SDL_FRect rect, int* exit_code)
{
    struct Texture result;

    /// Parameter checking
    if (path == NULL)
    {
        print_error("`load_texture()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (exit_code == NULL)
        print_warning("`load_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (rect.w == 0 || rect.h == 0)
        print_warning("`load_texture()`: `rect`'s `x` or `y` is 0. Are you sure?", NON_SDL_ERROR);
    
    /// Texture loading
    result.texture = IMG_LoadTexture(graphics_layer.renderer, path);
    if (result.texture == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`load_texture()`: couldn't load the texture, replaced with null texture", IS_SDL_ERROR);
            result.texture = NULL_TEXTURE;
        }
        else
        {
            print_error("`load_texture()`: couldn't load the texture, and null texture is empty", IS_SDL_ERROR);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }

    result.rect = rect;
    *exit_code = EXIT_SUCCESS;
    return result;
}


void render_texture(const struct Texture* target)
{
    /* Param checking */
    if (target == NULL || target->texture == NULL)
    {
        print_error("`render_texture()` target or its `texture` is `NULL`", NON_SDL_ERROR);
        return;
    }
    if (target->rect.w == 0 || target->rect.h == 0)
        print_warning("`render_texture()`: target's `w` or `h` is 0. Are you sure this was supposed to happen?", NON_SDL_ERROR);
    if (target->rect.x + target->rect.w <= 0 ||
        target->rect.y + target->rect.h <= 0 ||
        target->rect.x >= RENDER_WIDTH ||
        target->rect.y >= RENDER_HEIGHT)
    {
        print_warning("`render_texture()`: texture rendering out of bounds", NON_SDL_ERROR);
        return; /// While it's not an error, why waste a draw call on something not seen anyway?
    }
    
    /* Rendering */
    SDL_RenderTexture(graphics_layer.renderer, target->texture, NULL, &target->rect);
}


void free_texture(struct Texture* target)
{
    if (target == NULL)
        return;

    if (target->texture != NULL && target->texture != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->texture);
        target->texture = NULL;
    }
    target->rect = (SDL_FRect){0.0, 0.0, 0.0, 0.0};
    return;
}

#endif /// TEXTURE_H