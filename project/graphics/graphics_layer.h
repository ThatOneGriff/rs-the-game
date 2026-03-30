#ifndef GRAPHICS_LAYER_H
#define GRAPHICS_LAYER_H

/* Headers */
#include <SDL3/SDL.h> /// SDL things.
#include <stdbool.h>  /// `bool screen_changed, force_render`.

#define WINDOW_TITLE "SDL Demo"
#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 720
#define WINDOW_RECT (SDL_FRect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}
#define RENDER_WIDTH  960
#define RENDER_HEIGHT 720
#define RENDER_RECT (SDL_FRect){0, 0, RENDER_WIDTH, RENDER_HEIGHT}
#define RENDER_TO_WINDOW_RATIO ((float)RENDER_WIDTH / WINDOW_WIDTH) /// Assumes the aspect ratios of render and window rectangles match.
#define WINDOW_TO_RENDER_RATIO ((float)WINDOW_WIDTH / RENDER_WIDTH) /// Assumes the aspect ratios of window and render rectangles match.


/* Struct */

struct Graphics_Layer
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  buffer;

    bool screen_changed;
    bool force_render;
};
extern struct Graphics_Layer graphics_layer; /// Singleton.


/* Predef */

void init_graphics_layer(int *const exit_code);
void free_graphics_layer(void);


#endif /// GRAPHICS_LAYER_H