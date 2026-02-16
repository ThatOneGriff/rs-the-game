#ifndef SHIFTING_TEXTURE_H
#define SHIFTING_TEXTURE_H

/* Headers */
#include <SDL3/SDL.h>                /// `SDL_FRect`, `SDL_Texture()`.
#include <stdbool.h>                 /// `bool freeze_shifting`.
#include "../../logic/logic_layer.h" /// `time_tick_ms`.


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
    time_span_ms step;
};


/* Predef */

struct Shifting_Texture init_shifting_texture(const SDL_FRect rect, const size_t max_count, const time_span_ms step, int *const exit_code);
void                    free_shifting_texture(struct Shifting_Texture *const target);
void add_to_shifting_texture(struct Shifting_Texture *const to, const char *const new_texture_path, int *const exit_code);
void render_shifting_texture(struct Shifting_Texture *const target);


#endif /// SHIFTING_TEXTURE_H