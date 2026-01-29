#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_Texture`, `SDL_FRect`.


/* Struct */

struct Texture
{
    SDL_Texture* texture;
    SDL_FRect    rect;
};


/* Predef */

struct Texture load_texture(const char* path, const SDL_FRect rect, int* exit_code);
void           free_texture(struct Texture* target);
void render_texture(const struct Texture* target);


#endif /// TEXTURE_H