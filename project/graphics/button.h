#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL.h>         /// SDL3.
#include <SDL3_ttf/SDL_ttf.h> /// SDL3_TTF.

#include "../debug.h"            /// Error printing.
#include "../helpers/helpers.h"  /// Helpers (`SDL_WHITE`).
#include "../helpers/geometry.h" /// `Vec2`.

#include "../text/border.h" /// Text w/ borders.
#include "../text/text.h"   /// Text.
#include "../graphics/texture.h" /// Rendering text into a texture.


/* Predef */

struct Button;
struct Button create_button(const char* text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int size, int* exit_code);
void            free_button(struct Button* target);


/* Struct */

/// As of now, `Button` is just a text, visually. TODO: add a bg rectangle.
/// IDEA: can instead be 2 `SDL_Texture`'s, as they (presumably) share coords. "Presumably" because coords may be slightly diff due to the text border.
struct Button
{
    struct Texture regular;
    struct Texture focused;
    bool is_focused;
};


/* Body */

struct Button create_button(const char* text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int size, int* exit_code)
{
    struct Button result;
    result.is_focused = false;

    /* Param checking */
    if (size == 0)
    {
        print_error("`init_button()`: `size` is 0", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (exit_code == NULL)
        print_warning("`init_button()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /* Regular texture */
    result.regular = create_text(text, inner_color, SDL_WHITE, screen_pos, size, 0,       exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_button()`: regular texture creation failure", NON_SDL_ERROR);
        return result;
    }

    /* Focused texture */
    result.focused = create_text(text, inner_color, SDL_WHITE, screen_pos, size, size/10, exit_code); /// WARNING: may result in a `0` border thickness, making the button appear not selected.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_button()`: focused texture creation failure", NON_SDL_ERROR);
        return result;
    }

    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_button(struct Button* target)
{
    target->is_focused = false;
    free_texture(&target->regular);
    free_texture(&target->focused);
}

#endif /// BUTTON_H