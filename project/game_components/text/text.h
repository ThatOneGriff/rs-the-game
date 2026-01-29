#pragma once
#ifndef TEXT_H
#define TEXT_H

/* SDL3 */
#include <SDL3/SDL.h> /// SDL3.

/* Helper headers */
#include "../../helpers/geometry.h" /// `Vec2`.


/* Predef */
struct Texture create_text(const char* text, const SDL_Color inner_color, const SDL_Color outer_color, const struct Vec2 screen_pos, const int size, const int border_thickness, int* exit_code);
void          _blit_8x(SDL_Surface* surf_out, SDL_Surface* surf_target, const int radius, const int x, const int y);
SDL_Surface*  _create_bordered_text_surface(const char* text, const int text_size, const int border_thickness, const SDL_Color inner_color, const SDL_Color outer_color);

#endif /// TEXT_H