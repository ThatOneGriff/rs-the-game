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

unsigned randint       (const unsigned min, const unsigned max);
         int randint_w_neg (         const int min,          const int max);
unsigned randint_except(      unsigned min,       unsigned max, const unsigned except);
unsigned randint_except_array(unsigned min,       unsigned max,       unsigned *const except_array, const unsigned short except_array_size);
SDL_Color    rand_color  (void);
float        rand_percent(const unsigned min, const unsigned max);
struct Vec2  rand_vec2   (const struct Vec2  min, const struct Vec2  max);


/* Body */

static time_t _seed = 0; /// Temporary value (time-initialized with the first function call).
unsigned randint(const unsigned min, const unsigned max)
{
    if (min > max)
        return 0;
    else if (min == max)
        return min;
    
    /// not seeded (happens only once)
    if (_seed == 0)
    {
        _seed = time(NULL);
        srand((unsigned)_seed); /// srand() only needs to be called once.
    }

    return (unsigned)rand() % (max-min+1) + min;
}


/// [-10, 10] = [-10+10, 10+10] - 10 = [0, 20] - 10
int randint_w_neg(const int min, const int max)
{
    const int diff_0 = 0-min;
    return (int)randint((unsigned)(min+diff_0), (unsigned)(max+diff_0)) - diff_0;
}


unsigned randint_except(unsigned min, unsigned max, const unsigned except)
{
    if (except == min) ++min;
    if (except == max) --max;
    if (except < min || except > max)
        return randint(min, max);

    unsigned result = randint(min, max-1);
    if (result >= except)
        ++result;
    
    return result;
}


unsigned randint_except_array(unsigned min, unsigned max, unsigned *const except_array, const unsigned short except_array_size)
{
    unsigned result = UINT_MAX;
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
float rand_percent(const unsigned min, const unsigned max)
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