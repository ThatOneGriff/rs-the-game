/* Related header */
#include "text.h"

/* SDL3 */
#include <SDL3/SDL.h>         /// SDL3.
#include <SDL3_ttf/SDL_ttf.h> /// SDL3_ttf.

/* C headers */
#include <stdlib.h> /// Exit codes.

/* Helper headers */
#include "../../debug.h"                   /// Error printing.
#include "../../deinit_stack.h"            /// Deinitialization stack.
#include "../../resources.h"               /// `MAIN_FONT_PATH`.
#include "../../graphics/graphics_layer.h" /// `graphics_layer.renderer` for `SDL_CreateTextureFromSurface()`.
#include "../../helpers/geometry.h"        /// `Vec2`.

/* Text-related headers */
#include "../../game_components/graphics/texture.h" /// Making a texture from text surface.


/* Predef */
struct Texture create_text(const char* text, const SDL_Color inner_color, const SDL_Color outer_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int* exit_code);
static void         _blit_8x(SDL_Surface* surf_out, SDL_Surface* surf_target, const unsigned int radius, const int x, const int y);
static SDL_Surface* _create_bordered_text_surface(const char* text, const unsigned int text_size, const unsigned int border_thickness, const SDL_Color inner_color, const SDL_Color outer_color);


/* Body */

struct Texture create_text(const char* text, const SDL_Color inner_color, const SDL_Color outer_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int* exit_code)
{
    struct Texture result;
    result.texture = NULL;
    result.rect    = (SDL_FRect){0.0, 0.0, 0.0, 0.0};

    /// Checking args
    if (size == 0)
    {
        print_error("`create_text()`: `size` == 0", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (exit_code == NULL)
        print_warning("`create_text()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);

    /// Creating text surface
    SDL_Surface* text_surf = _create_bordered_text_surface(text, size, border_thickness, inner_color, outer_color);
    if (text_surf == NULL)
    {
        print_error("`create_text()`: couldn't create `text_surf`", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Creating texture from text surface
    result.texture = SDL_CreateTextureFromSurface(graphics_layer.renderer, text_surf);
    if (result.texture == NULL)
    {
        print_error("`create_text()`: couldn't create a resulting texture from `text_surf`", IS_SDL_ERROR);
        SDL_DestroySurface(text_surf);
        text_surf = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Coords (size)
    result.rect.w = (float)text_surf->w;
    result.rect.h = (float)text_surf->h;
    SDL_DestroySurface(text_surf);
    text_surf = NULL;

    /// Coords (position)
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


/// Helper for midpoint cicle algorithm. Explanation in `_create_bordered_text_surface()`.
static void _blit_8x(SDL_Surface* surf_out, SDL_Surface* surf_target, const unsigned int radius, const int x, const int y)
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
            SDL_BlitSurface(surf_out, NULL, surf_target, &(SDL_Rect){(int)radius + x*x_mult, (int)radius + y*y_mult, surf_out->w, surf_out->h});
    
    for (int y_mult = -1; y_mult <= 1; y_mult += 2)
        for (int x_mult = -1; x_mult <= 1; x_mult += 2)
            SDL_BlitSurface(surf_out, NULL, surf_target, &(SDL_Rect){(int)radius + y*y_mult, (int)radius + x*x_mult, surf_out->w, surf_out->h});
}


static SDL_Surface* _create_bordered_text_surface(const char* text, const unsigned int text_size, const unsigned int border_thickness, const SDL_Color inner_color, const SDL_Color outer_color)
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
    TTF_Font *font = TTF_OpenFont(MAIN_FONT_PATH, (float)text_size); /// IDEA: loading the font each time we create a text is actually wasteful?
    if (font == NULL)
    {
        print_error("`_create_bordered_text_surface()`: error loading the font", IS_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return NULL;
    }
    add_to_deinit_stack(&deinit_stack, font, (void (*)(void*))TTF_CloseFont);

    /// `0` arg for auto-determined length.
    SDL_Surface* surf_in = TTF_RenderText_Blended(font, text, 0, inner_color);
    if (surf_in == NULL)
    {
        print_error("`_create_bordered_text_surface()`: couldn't render `surf_in`", IS_SDL_ERROR);
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
        print_error("`_create_bordered_text_surface()`: couldn't render `surf_out`", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return NULL;
    }
    add_to_deinit_stack(&deinit_stack, surf_out, (void (*)(void*))SDL_DestroySurface);
    
    /// A surface of the same pixel type as TTF-related things.
    SDL_Surface* result = SDL_CreateSurface((int)surf_out->w + (int)border_thickness*2, (int)surf_out->h + (int)border_thickness*2, SDL_PIXELFORMAT_ARGB32);
    if (result == NULL)
    {
        print_error("`_create_bordered_text_surface()`: couldn't create `result` surface", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return NULL;
    }
    /// Not added to the deinit stack.

    /* == Midpoint cicle algorithm == */

    /// It draws 1/8th of a circle, and then reflects the rest of it.
    /// Furthermore, it requires no complex radian maths: only + and -.
    /// Works on all circle sizes.

    int x = 0;
    int y = (int)border_thickness;
    int d = 3 - 2 * (int)border_thickness; /// I can only assume it's difference / step.

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
    SDL_BlitSurface(surf_in, NULL, result, &(SDL_Rect){(int)border_thickness, (int)border_thickness, surf_in->w, surf_in->h});

    flush_deinit_stack(&deinit_stack); /// `flush` because nothing, except the `result`, will be used.
    return result;
}