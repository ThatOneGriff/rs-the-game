#ifndef RANDOM_H
#define RANDOM_H

/* Headers */
#include <SDL3/SDL.h> /// `rand_color()`.
#include "geometry.h" /// `struct Vec2`.


/* Prefef */

unsigned randint       (const unsigned min, const unsigned max);
         int randint_w_neg (         const int min,          const int max);
unsigned randint_except(      unsigned min,       unsigned max, const unsigned        except);
unsigned randint_except_array(unsigned min,       unsigned max,       unsigned *const except_array, const unsigned short except_array_size);
SDL_Color    rand_color  (void);
float        rand_percent(const unsigned min, const unsigned max);
struct Vec2  rand_vec2   (const struct Vec2  min, const struct Vec2  max);


#endif /// RANDOM_H