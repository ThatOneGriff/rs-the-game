#pragma once
#ifndef BUTTON_H
#define BUTTON_H

/* C headers */
#include <stdbool.h> /// `bool is_focused`.

/* Helpers */
#include "../../helpers/geometry.h" /// `struct Vec2`.

/* Components */
#include "../graphics/texture.h"    /// Rendering text into a texture.


/* Struct */

struct Button
{
    struct Texture regular_texture;
    struct Texture focused_texture;
    bool is_focused;
};


/* Predef */

struct Button create_button(const char* text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int* exit_code);
void          render_button(const struct Button* target);
void            free_button(struct Button* target);


#endif /// BUTTON_H