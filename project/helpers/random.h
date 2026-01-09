#pragma once
#ifndef RANDOM_H
#define RANDOM_H

/* Headers */
#include <SDL3/SDL.h> /// Rand `SDL_Color`
#include <stdio.h>    /// I/O.
#include <time.h>     /// Time-based random seeding.
#include "geometry.h" /// `struct Vec2`


/* Prefef */

unsigned int randint       (const unsigned int min, const unsigned int max);
         int randint_w_neg (         const int min,          const int max);
unsigned int randint_except(      unsigned int min,       unsigned int max, const unsigned int except);
SDL_Color    rand_color(void);
float        rand_percent(const unsigned int min, const unsigned int max);
struct Vec2  rand_vec2(const struct Vec2 min, const struct Vec2 max);



/* Body */

unsigned int _seed = 0; /// Temporary value (time-initialized with the first function call).
unsigned int randint(const unsigned int min, const unsigned int max)
{
    if (min > max)
    {
        fprintf(stderr, "~ Incorrect `randint()` arguments: %u %u\n", min, max);
        return 0;
    }
    else if (min == max)
        return min;
    
    /// not seeded (happens only once)
    if (_seed == 0)
    {
        _seed = time(NULL);
        srand(_seed); /// srand() only needs to be called once.
    }

    return rand() % (max-min+1) + min;
}


/// [-10, 10] = [-10+10, 10+10] - 10 = [0, 20] - 10
int randint_w_neg(const int min, const int max)
{
    const int diff_0 = 0-min;
    return (int)randint((unsigned)(min+diff_0), (unsigned)(max+diff_0)) - diff_0;
}


unsigned int randint_except(unsigned int min, unsigned int max, const unsigned int except)
{
    if (except == min) ++min;
    if (except == max) --max;
    if (except < min || except > max)
        return randint(min, max);

    unsigned int result = randint(min, max-1);
    if (result >= except)
        ++result;
    
    return result;
}


SDL_Color rand_color(void)
{
    return (SDL_Color){randint(0, 255), randint(0, 255), randint(0, 255), 0};
}


/// [95, 105] = [0.95, 1.05]
float rand_percent(const unsigned int min, const unsigned int max)
{
    return (float)(randint(min, max)) / 100.0;
}


struct Vec2 rand_vec2(const struct Vec2 min, const struct Vec2 max)
{
    return vec2(randint_w_neg(min.x, max.x), randint_w_neg(min.y, max.y));
}

#endif /// RANDOM_H