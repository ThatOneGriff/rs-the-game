#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL.h>         /// SDL3.
#include <SDL3_ttf/SDL_ttf.h> /// SDL3_TTF.

#include "../debug.h"            /// Error printing.
#include "../helpers/geometry.h" /// `Vec2`.

#include "../text/border.h" /// Text w/ borders.
#include "../text/text.h"   /// Text.
#include "texture.h" /// Rendering text into a texture.


/* Predef */

struct Button;
struct Button create_button(const char* text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int size, int* exit_code);
void          render_button(const struct Button* target);
void            free_button(struct Button* target);


/* Struct */

/// As of now, `Button` is just a text, visually. TODO: add a bg rectangle.
struct Button
{
    struct Texture regular_texture;
    struct Texture focused_texture;
    bool is_focused;
};


/* Body */

struct Button create_button(const char* text, const SDL_Color inner_color, struct Vec2 screen_pos, const unsigned int size, int* exit_code)
{
    struct Button result;
    result.is_focused = false;

    /* Param checking */
    if (size == 0)
    {
        print_error("`create_button()`: `size` is 0", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (exit_code == NULL)
        print_warning("`create_button()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /* regular_texture texture */
    result.regular_texture = create_text(text, inner_color, (SDL_Color){188,204,220,255}, screen_pos, size, size/20, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`create_button()`: `regular_texture` creation failure", NON_SDL_ERROR);
        return result;
    }

    /* focused_texture texture */
    const struct Vec2 border_adjusted_screen_pos = vec2(screen_pos.x - size/20, screen_pos.y - size/20); /// So that focusing doesn't make the button jitter.
    result.focused_texture = create_text(text, inner_color, (SDL_Color){255,255,255,255}, border_adjusted_screen_pos, size, size/10, exit_code); /// WARNING: may result in a `0` border thickness, making the button appear not selected.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`create_button()`: `focused_texture` creation failure", NON_SDL_ERROR);
        free_texture(&result.regular_texture);
        return result;
    }

    *exit_code = EXIT_SUCCESS;
    return result;
}


void render_button(const struct Button* target)
{
    /* Param checking */
    if (target == NULL)
    {
        print_error("`render_button()` target is `NULL`", NON_SDL_ERROR);
        return;
    }
    
    /* Rendering */
    if (target->is_focused)
        render_texture(&target->focused_texture);
    else
        render_texture(&target->regular_texture);
}


void free_button(struct Button* target)
{
    target->is_focused = false;
    free_texture(&target->regular_texture);
    free_texture(&target->focused_texture);
}

#endif /// BUTTON_H