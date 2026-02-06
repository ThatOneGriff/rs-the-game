#pragma once
#ifndef TEXT_H
#define TEXT_H

/* Headers */
#include <SDL3/SDL.h> /// SDL3.
#include "../../helpers/geometry.h" /// `Vec2`.


/* Predef */

struct Texture create_text(const char* text, const SDL_Color inner_color, const SDL_Color outer_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int* exit_code);


#endif /// TEXT_H