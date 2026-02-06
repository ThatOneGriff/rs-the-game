/* Related header */
#include "path.h"

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_FRect()`.

/* C headers */
//#include <stdbool.h>  /// `bool is_loop` (For now, every path is a loop).
#include <stdlib.h>   /// `*alloc()`.

/* Helpers */
#include "../../debug.h" /// Error printing.


/* Predef */

struct Path  new_path(const SDL_FRect* points, const size_t pt_count, int* exit_code);
void        free_path(struct Path* target);


/* Body */

struct Path new_path(const SDL_FRect* points, const size_t pt_count, int* exit_code)
{
    struct Path result;
    result.points = NULL;
    result.pt_count = 0; /// Temporary value to be changed once memory is successfully allocated.

    /// Param checking
    if (exit_code == NULL)
        print_warning("`init_path()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (pt_count == 0)
    {
        print_error("`init_path()`: `pt_count` arg is 0. `Path` is not dynamic-memory", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (points == NULL)
    {
        print_error("`init_path()`: `points` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Object creation
    result.points = calloc(pt_count, sizeof(SDL_FRect));
    if (result.points == NULL)
    {
        print_error("`init_path()`: couldn't allocate memory to path points", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    for (size_t i = 0; i < pt_count; i++)
        result.points[i] = points[i];
    result.pt_count = pt_count;
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_path(struct Path* target)
{
    if (target == NULL)
        return;
    
    if (target->points != NULL)
    {
        free(target->points);
        target->points = NULL;
    }

    target->pt_count = 0;
}