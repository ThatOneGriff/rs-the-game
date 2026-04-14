#ifndef BUTTON_H
#define BUTTON_H

/* Headers */
#include <stdbool.h>                /// `bool is_focused`.
#include "ui_common.h"              /// Common framework for `struct Switch` and `struct Button`.
#include "../graphics/sprite.h"     /// Rendering text into a texture.
#include "../../helpers/geometry.h" /// `struct Vec2`.


/* Struct */

struct Button
{
    /// [!!!] UNTOUCHABLE [!!!]
    bool is_focused;

    void* neighbors[UI_NEIGHBORS_COUNT];
    /// [!!!] UNTOUCHABLE [!!!]

    struct Sprite regular_texture;
    struct Sprite focused_texture;
};


/* Predef */

struct Button  create_button(const char *const text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int text_size, const unsigned int border_thickness, int *const exit_code);
void add_neighbors_to_button(struct Button *const target, void *const up, void *const down, void *const left, void *const right);
void           render_button(const struct Button *const target);
void             free_button(      struct Button *const target);


#endif /// BUTTON_H