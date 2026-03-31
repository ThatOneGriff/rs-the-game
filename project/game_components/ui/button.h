#ifndef BUTTON_H
#define BUTTON_H

/* Headers */
#include <stdbool.h>                /// `bool is_focused`.
#include "ui_neighbor.h"            /// For arrow navigation.
#include "../graphics/sprite.h"     /// Rendering text into a texture.
#include "../../helpers/geometry.h" /// `struct Vec2`.


/* Struct */

struct Button
{
    bool is_focused;
    
    struct Sprite regular_texture;
    struct Sprite focused_texture;

    struct UI_Neighbor up;
    struct UI_Neighbor down;
    struct UI_Neighbor left;
    struct UI_Neighbor right;
};


/* Predef */

struct Button create_button(const char *const text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int *const exit_code);
void          render_button(const struct Button *const target);
void            free_button(      struct Button *const target);


#endif /// BUTTON_H