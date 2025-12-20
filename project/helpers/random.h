#pragma once
#ifndef RANDOM_H
#define RANDOM_H

#include <stdio.h> /// I/O.
#include <time.h>  /// Time-based random seeding.


/* Prefef */

unsigned int randint     (const unsigned int min, const unsigned int max);
unsigned int randint_except(    unsigned int min,       unsigned int max, const unsigned int except);
float        rand_percent(const unsigned int min, const unsigned int max);


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


/// [95, 105] = [0.95, 1.05]
float rand_percent(const unsigned int min, const unsigned int max)
{
    return (float)(randint(min, max)) / 100.0;
}

#endif /// RANDOM_H