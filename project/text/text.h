#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <SDL3/SDL.h> /// SDL3.
#include <SDL3_ttf/SDL_ttf.h> /// SDL3_TTF.

#include "border.h"       /// Text w/ borders.
#include "../helpers/geometry.h" /// `Vec2`.
#include "../graphics/graphics_layer.h" /// `SDL_CreateTextureFromSurface()`.
#include "../game_components/texture.h" /// Rendering text into a texture.
#include "../resources.h" /// Font path(s).


/* Predef */

struct Texture create_text(const char* text, const SDL_Color inner_color, const SDL_Color outer_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int* exit_code);


/* Body */

struct Texture create_text(const char* text, const SDL_Color inner_color, const SDL_Color outer_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int* exit_code)
{
    struct Texture result;

    /* Checking args */

    if (size == 0)
    {
        print_error("`create_text()`: `size` is 0", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (exit_code == NULL)
        print_warning("`create_text()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);

    /* Creating the text */

    SDL_Surface* text_surf = create_bordered_text_surface(text, size, border_thickness, inner_color, outer_color);
    if (text_surf == NULL)
    {
        print_error("`create_text()`: couldn't create `text_surf`", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.texture = SDL_CreateTextureFromSurface(graphics_layer.renderer, text_surf);
    if (result.texture == NULL)
    {
        print_error("`create_text()`: couldn't create a resulting texture from `text_surf`", IS_SDL_ERROR);
        SDL_DestroySurface(text_surf);
        text_surf = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.rect.w = (float)text_surf->w;
    result.rect.h = (float)text_surf->h;
    SDL_DestroySurface(text_surf);
    text_surf = NULL;

    if (screen_pos.x == X_AUTO_CENTER)
        result.rect.x = center_x(result.rect.w);
    else
        result.rect.x = screen_pos.x;
    
    if (screen_pos.y == Y_AUTO_CENTER)
        result.rect.y = center_y(result.rect.h);
    else
        result.rect.y = screen_pos.y;

    *exit_code = EXIT_SUCCESS;
    return result;
}

#endif /// TEXT_H