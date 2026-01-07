#pragma once
#ifndef BORDER_H
#define BORDER_H

/* SDL3 */
#include <SDL3/SDL.h>         /// SDL3.
#include <SDL3_ttf/SDL_ttf.h> /// SDL3_ttf.

/* Helper headers */
#include "../debug.h"        /// Error output.
#include "../deinit_stack.h" /// Deinitialization stack.
#include "../resources.h"    /// `MAIN_FONT_PATH`.


/* Predef */

void _blit_8x(SDL_Surface* surf_out, SDL_Surface* surf_target, const int radius, const int x, const int y);
SDL_Surface* create_bordered_text_surface(const char* text, const float text_size, const unsigned int border_thickness, const SDL_Color inner_color, const SDL_Color outer_color);


/* Body */

/// Helper for midpoint cicle algorithm. Explanation in `create_bordered_text_surface()`.
void _blit_8x(SDL_Surface* surf_out, SDL_Surface* surf_target, const int radius, const int x, const int y)
{
    if (surf_out == NULL || surf_target == NULL)
    {
        print_error("`_blit_8x()`: `NULL` surface arg(s)", NON_SDL_ERROR);
        return;
    }
    if (radius == 0)
        return;

    for (int x_mult = -1; x_mult <= 1; x_mult += 2)
        for (int y_mult = -1; y_mult <= 1; y_mult += 2)
            SDL_BlitSurface(surf_out, NULL, surf_target, &(SDL_Rect){radius + x*x_mult, radius + y*y_mult, surf_out->w, surf_out->h});
    
    for (int y_mult = -1; y_mult <= 1; y_mult += 2)
        for (int x_mult = -1; x_mult <= 1; x_mult += 2)
            SDL_BlitSurface(surf_out, NULL, surf_target, &(SDL_Rect){radius + y*y_mult, radius + x*x_mult, surf_out->w, surf_out->h});
}


SDL_Surface* create_bordered_text_surface(const char* text, const float text_size, const unsigned int border_thickness, const SDL_Color inner_color, const SDL_Color outer_color)
{
    /// Deinit stack
    int exit_code = EXIT_SUCCESS;
    struct Deinit_Stack deinit_stack = new_deinit_stack(3, &exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return NULL;
    }

    /// Font
    TTF_Font *font = TTF_OpenFont(MAIN_FONT_PATH, text_size); /// IDEA: loading the font each time we create a text is actually wasteful?
    if (font == NULL)
    {
        print_error("`create_bordered_text_surface()`: error loading the font", IS_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return NULL;
    }
    add_to_deinit_stack(&deinit_stack, font, (void (*)(void*))TTF_CloseFont);

    /// `0` arg for auto-determined length.
    SDL_Surface* surf_in = TTF_RenderText_Blended(font, text, 0, inner_color);
    if (surf_in == NULL)
    {
        print_error("`create_bordered_text_surface()`: couldn't render `surf_in`", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return NULL;
    }
    if (border_thickness == 0) /// No border, so no need to advance
    {
        flush_deinit_stack(&deinit_stack);
        return surf_in;
    }
    add_to_deinit_stack(&deinit_stack, surf_in, (void (*)(void*))SDL_DestroySurface);

    /// `0` arg for auto-determined length.
    SDL_Surface* surf_out = TTF_RenderText_Blended(font, text, 0, outer_color);
    if (surf_out == NULL)
    {
        print_error("`create_bordered_text_surface()`: couldn't render `surf_out`", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return NULL;
    }
    add_to_deinit_stack(&deinit_stack, surf_out, (void (*)(void*))SDL_DestroySurface);
    
    /// A surface of the same pixel type as TTF-related things.
    SDL_Surface* result = SDL_CreateSurface(surf_out->w + border_thickness*2, surf_out->h + border_thickness*2, SDL_PIXELFORMAT_ARGB32);
    if (result == NULL)
    {
        print_error("`create_bordered_text_surface()`: couldn't create `result` surface", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return NULL;
    }
    /// Not added to the deinit stack.

    /* == Midpoint cicle algorithm == */

    /// It draws 1/8th of a circle, and then reflects the rest of it.
    /// Furthermore, it requires no complex radian maths: only + and -.
    /// Works on all circle sizes.

    int x = 0;
    int y = border_thickness;
    int d = 3-2*border_thickness; /// I can only assume it's difference / step.

    _blit_8x(surf_out, result, border_thickness, x, y); /// We need 1 run on initial {x, y}.
    while (y >= x)
    {
        ++x;
        if (d > 0)
        {
            --y;
            d = d + 4*(x-y) + 10; /// What's 10 for?
        }
        else
            d = d + 4*x + 6;
        
        _blit_8x(surf_out, result, border_thickness, x, y);
    }

    /// Slapping the inner surface in center of the outer.
    SDL_BlitSurface(surf_in, NULL, result, &(SDL_Rect){border_thickness, border_thickness, surf_in->w, surf_in->h});

    flush_deinit_stack(&deinit_stack);
    return result;
}

#endif /// BORDER_H