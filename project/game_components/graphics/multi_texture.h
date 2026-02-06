#pragma once
#ifndef MULTI_TEXTURE_H
#define MULTI_TEXTURE_H

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_FRect()`, `SDL_Texture`.


/* Struct */

struct Multi_Texture
{
    SDL_Texture* texture;
    SDL_FRect*   rects;
    size_t cur_count;
    size_t max_count;
};


/* Predef */

struct Multi_Texture load_multi_texture(const char* texture_path, const size_t max_count, int* exit_code);
void                 free_multi_texture(struct Multi_Texture* target);
void add_to_multi_texture(struct Multi_Texture* to, const SDL_FRect new_rects, int* exit_code);
void render_multi_texture(const struct Multi_Texture* target);


#endif /// MULTI_TEXTURE_H