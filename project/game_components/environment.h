#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_FRect`, `SDL_Texture`.

/* Related headers */
#include "movement/move_component.h"    /// Moving textures.


/* Struct */

/// This is a combination of `Shifting_Texture` and `Multi_Texture`:
/// a set of textures, going along a path, randomized with each iteration.
struct Environment
{
    SDL_Texture** textures;
    unsigned short        texture_count;
    unsigned short*       cur_texture_indexes;

    SDL_FRect*    rects; /// Exists solely for manipulation by `move_component`.
    unsigned short        object_count;

    struct Move_Component* move_component;
};


/* Predef */

struct Environment        new_environment(char **const texture_paths, const unsigned short texture_count, const unsigned short object_count, int *const exit_code);
void couple_move_component_to_environment(struct Environment *const to, struct Move_Component *const move_component, const struct Vec2 max_offset, int *const exit_code);
void                     free_environment(struct Environment *const target);
void        render_environment(const struct Environment *const target);
void partly_render_environment(const struct Environment *const target, const unsigned short min_path_pt, unsigned short max_path_pt);


#endif /// ENVIRONMENT_H