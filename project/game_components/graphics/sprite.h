#ifndef TEXTURE_H
#define TEXTURE_H

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_Texture`, `SDL_FRect`.


/* Struct */

struct Sprite
{
    SDL_Texture* texture;
    SDL_FRect    rect;
};


/* Predef */

struct Sprite load_sprite(const char *const path, const SDL_FRect rect, int *const exit_code);
void          free_sprite(struct Sprite *const target);
void   render_sprite(const struct Sprite *const target);


#endif /// TEXTURE_H