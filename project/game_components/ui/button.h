#ifndef BUTTON_H
#define BUTTON_H

/* C headers */
#include <stdbool.h> /// `bool is_focused`.

/* Helpers */
#include "../../helpers/geometry.h" /// `struct Vec2`.

/* Components */
#include "../graphics/sprite.h"    /// Rendering text into a texture.


/* Struct */

struct Button
{
    struct Sprite regular_texture;
    struct Sprite focused_texture;
    bool is_focused;

    struct Button* up;
    struct Button* down;
    struct Button* left;
    struct Button* right;
};


/* Predef */

struct Button  create_button(const char *const text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int *const exit_code);
void add_neighbors_to_button(      struct Button *const target, struct Button *const up, struct Button *const down, struct Button *const left, struct Button *const right);
void           render_button(const struct Button *const target);
void             free_button(      struct Button *const target);


#endif /// BUTTON_H