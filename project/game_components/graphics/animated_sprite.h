#ifndef SHIFTING_TEXTURE_H
#define SHIFTING_TEXTURE_H

/* Headers */
#include <SDL3/SDL.h>                /// `SDL_FRect`, `SDL_Texture()`.
#include <stdbool.h>                 /// `bool freeze_shifting`.
#include "../../logic/logic_layer.h" /// `time_tick_ms`.


/* Struct */

struct Animated_Sprite
{
    bool freeze_shifting;

    SDL_Texture** frames;
    SDL_FRect     rect;
    unsigned short cur_count;
    unsigned short max_count;
    unsigned short i;

    time_tick_ms latest_change;
    time_span_ms step;
};


/* Predef */

struct Animated_Sprite init_animated_sprite(const SDL_FRect rect, const unsigned short max_count, const time_span_ms step, int *const exit_code);
void                   free_animated_sprite(struct Animated_Sprite *const target);
void add_to_animated_sprite(struct Animated_Sprite *const to, const char *const new_texture_path, int *const exit_code);
void render_animated_sprite(struct Animated_Sprite *const target);


#endif /// SHIFTING_TEXTURE_H