#ifndef RANDOM_H
#define RANDOM_H

/* Headers */
#include <SDL3/SDL.h> /// `rand_color()`.
#include "geometry.h" /// `struct Vec2`.


/* Prefef */

unsigned int randint       (const unsigned int min, const unsigned int max);
         int randint_w_neg (         const int min,          const int max);
unsigned int randint_except(      unsigned int min,       unsigned int max, const unsigned int except);
SDL_Color    rand_color  (void);
float        rand_percent(const unsigned int min, const unsigned int max);
struct Vec2  rand_vec2   (const struct Vec2  min, const struct Vec2  max);


#endif /// RANDOM_H