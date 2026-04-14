/* Related header */
#include "random.h"

/* Headers */
#include <SDL3/SDL.h> /// `rand_color()`.
#include <stdbool.h>  /// `bool is_in_except_array`.
#include <stdio.h>    /// I/O.
#include <stdlib.h>   /// `rand()`.
#include <time.h>     /// Time-based random seeding.
#include "geometry.h" /// `struct Vec2`.


/* Prefef */

unsigned int randint       (const unsigned int min, const unsigned int max);
         int randint_w_neg (         const int min,          const int max);
unsigned int randint_except(      unsigned int min,       unsigned int max, const unsigned int except);
unsigned int randint_except_array(unsigned int min,       unsigned int max,       unsigned int *const except_array, const unsigned short except_array_size);
SDL_Color    rand_color  (void);
float        rand_percent(const unsigned int min, const unsigned int max);
struct Vec2  rand_vec2   (const struct Vec2  min, const struct Vec2  max);


/* Body */

static time_t _seed = 0; /// Temporary value (time-initialized with the first function call).
unsigned int randint(const unsigned int min, const unsigned int max)
{
    if (min > max)
        return 0;
    else if (min == max)
        return min;
    
    /// not seeded (happens only once)
    if (_seed == 0)
    {
        _seed = time(NULL);
        srand((unsigned int)_seed); /// srand() only needs to be called once.
    }

    return (unsigned)rand() % (max-min+1) + min;
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


unsigned int randint_except_array(unsigned int min, unsigned int max, unsigned int *const except_array, const unsigned short except_array_size)
{
    unsigned int result = UINT_MAX;
    bool is_in_except_array = false;
    do
    {
        result = randint(min, max);
        is_in_except_array = false;
        for (unsigned short i = 0; i < except_array_size; i++)
        {
            if (result == except_array[i])
            {
                is_in_except_array = true;
                break;
            }
        }
    }
    while (is_in_except_array);
    return result;
}


SDL_Color rand_color(void)
{
    return (SDL_Color){(Uint8)randint(0, 255), (Uint8)randint(0, 255), (Uint8)randint(0, 255), (Uint8)0};
}


/// [95, 105] = [0.95, 1.05]
float rand_percent(const unsigned int min, const unsigned int max)
{
    return (float)(randint(min, max)) / 100.0f;
}


/// WARNING: casts `float` to `int`.
struct Vec2 rand_vec2(const struct Vec2 min, const struct Vec2 max)
{
    return vec2(
        (float)randint_w_neg((int)min.x, (int)max.x),
        (float)randint_w_neg((int)min.y, (int)max.y)
    );
}