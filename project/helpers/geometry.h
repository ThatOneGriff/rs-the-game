#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

/* Headers */
#include <SDL3/SDL.h> /// `SDL_FRect`.
#include <stdbool.h>  /// `bool have_x_overlap()`.
#include "../game_components/movement/path.h" /// Path manipulation.
#include "../graphics/graphics_layer.h"       /// `RENDER_WIDTH/HEIGHT`.


/* Structs */

struct Vec2
{
    float x;
    float y;
};
#define X_AUTO_CENTER -1
#define Y_AUTO_CENTER -1


/* Predef */

#define center_x(w)  (float)(RENDER_WIDTH  - w) / 2.0f
#define center_y(h)  (float)(RENDER_HEIGHT - h) / 2.0f
#define reflect_x(x) (float)(RENDER_WIDTH  - x)
#define reflect_y(y) (float)(RENDER_HEIGHT - y) /// Unused for now.
#define vec2(x,y) (struct Vec2){x, y}

struct Path flipped_path_x(const struct Path path);
bool  have_x_overlap  (const SDL_FRect rect1, const SDL_FRect rect2);
float distance_between(const SDL_FRect rect1, const SDL_FRect rect2);

#endif /// GEOMETRY_H