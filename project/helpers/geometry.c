/* Related header */
#include "geometry.h"

/* Headers */
#include <SDL3/SDL.h>                         /// `SDL_FRect`.
#include <stdbool.h>                          /// `bool have_x_overlap()`.
#include <stdlib.h>                           /// `*alloc()`.
#include "../debug.h"                         /// Error message printing.
#include "../game_components/movement/path.h" /// Path manipulation.
#include "../graphics/graphics_layer.h"       /// `RENDER_WIDTH/HEIGHT`.


/* Body */

struct Path flipped_path_x(const struct Path path)
{
    struct Path result;
    result.points = malloc(path.pt_count * sizeof(SDL_FRect));
    if (result.points == NULL)
    {
        print_error("`flipped_path_x()`: couldn't allocate memory", NON_SDL_ERROR);
        return result; /// TODO: add `exit_code`.
    }
    for (size_t i = 0; i < path.pt_count; i++)
    {
        result.points[i] = path.points[i];
        result.points[i].x = reflect_x(path.points[i].x) - path.points[i].w;
    }
    result.pt_count = path.pt_count;
    return result;
}

bool have_x_overlap(const SDL_FRect rect1, const SDL_FRect rect2)
{
    return (
        ((rect1.x < rect2.x) && (rect1.x + rect1.w > rect2.x))
     || ((rect2.x < rect1.x) && (rect2.x + rect2.w > rect1.x))
    );
}

float distance_between(const SDL_FRect rect1, const SDL_FRect rect2)
{
    const float dist1 = (rect1.x - rect2.x + rect2.w);
    const float dist2 = (rect2.x - rect1.x + rect1.w);
    if (dist1 < 0)
        return dist2;
    else if (dist2 < 0)
        return dist1;
    else
        return (dist1 < dist2 ? dist1 : dist2);
}

float center_x(const float w)
{
    return (RENDER_WIDTH - w) / (float)2.0;
}

float center_y(const float h)
{
    return (RENDER_HEIGHT - h) / (float)2.0;
}

float reflect_x(const float x)
{
    return RENDER_WIDTH - x;
}

struct Vec2 vec2(const float x, const float y)
{
    struct Vec2 result;
    result.x = x;
    result.y = y;
    return result;
}