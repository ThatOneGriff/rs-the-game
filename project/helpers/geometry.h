#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

/* Headers */
#include <SDL3/SDL.h> /// `SDL_FRect`.
#include <stdbool.h>  /// `bool have_x_overlap()`.
#include "../game_components/movement/path.h" /// Path manipulation.


/* Structs */

struct Vec2
{
    float x;
    float y;
};
#define X_AUTO_CENTER -1
#define Y_AUTO_CENTER -1


/* Predef */

/// REDO: rewrite using macros.

struct Path flipped_path_x(const struct Path path);
bool  have_x_overlap  (const SDL_FRect rect1, const SDL_FRect rect2);
float distance_between(const SDL_FRect rect1, const SDL_FRect rect2);
float center_x (const float w);
float center_y (const float h);
float reflect_x(const float x);
struct Vec2 vec2(const float x, const float y);

#endif /// GEOMETRY_H