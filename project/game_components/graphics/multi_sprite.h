#ifndef MULTI_TEXTURE_H
#define MULTI_TEXTURE_H

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_FRect()`, `SDL_Texture`.


/* Struct */

struct Multi_Sprite
{
    SDL_Texture* texture;
    SDL_FRect*   rects;
    size_t cur_count;
    size_t max_count;
};


/* Predef */

struct Multi_Sprite load_multi_sprite(const char *const texture_path, const size_t max_count, int *const exit_code);
void                free_multi_sprite(struct Multi_Sprite *const target);
void add_to_multi_sprite(struct Multi_Sprite *const to, const SDL_FRect new_rects, int *const exit_code);
void render_multi_sprite(const struct Multi_Sprite *const target);


#endif /// MULTI_TEXTURE_H