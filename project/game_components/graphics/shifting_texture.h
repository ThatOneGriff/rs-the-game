#pragma once
#ifndef SHIFTING_TEXTURE_H
#define SHIFTING_TEXTURE_H

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_FRect`, `SDL_Texture()`.

/* C headers */
#include <stdbool.h> /// `bool freeze_shifting`.

/* Helper headers */
#include "../../logic/logic_layer.h"       /// `time_tick_ms`.


/* Struct */

struct Shifting_Texture
{
    bool freeze_shifting;

    SDL_Texture** textures;
    SDL_FRect     rect;
    size_t cur_count;
    size_t max_count;
    size_t i;

    time_tick_ms latest_change;
    time_tick_ms step;
};


/* Predef */

struct Shifting_Texture init_shifting_texture(const SDL_FRect rect, const size_t max_count, const time_tick_ms step, int* exit_code);
void                    free_shifting_texture(struct Shifting_Texture* target);
void add_to_shifting_texture(struct Shifting_Texture* to, const char* new_texture_path, int* exit_code);
void render_shifting_texture(struct Shifting_Texture* target);

#endif /// SHIFTING_TEXTURE_H