#ifndef PSEUDO_BUTTON_H
#define PSEUDO_BUTTON_H

/* Headers */
#include "../graphics/sprite.h"     /// `struct Sprite` members.
#include "../../helpers/geometry.h" /// `struct Vec2`.


/// What is it good for?
/// You see, a `Switch` contains buttons as members.
/// However, its buttons don't need neighbors and `bool is_focused`, which together give (8 * 4 + 8) = 40 bits of wasted memory PER MEMBER.
/// Thus, we needed a way to reduce those costs.


/* Struct */

struct Pseudo_Button
{
    /// Activity status is determined by the `Switch`.

    struct Sprite regular_texture;
    struct Sprite focused_texture;
};


/* Predef */

struct Pseudo_Button create_pseudo_button(const char *const text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int text_size, const unsigned int border_thickness, int *const exit_code);
void                   free_pseudo_button(struct Pseudo_Button *const target);

#endif /// PSEUDO_BUTTON_H