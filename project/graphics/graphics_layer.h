#ifndef GRAPHICS_LAYER_H
#define GRAPHICS_LAYER_H

/* Headers */
#include <SDL3/SDL.h> /// SDL things.
#include <stdbool.h>  /// `bool screen_changed, force_render`.

#define WINDOW_TITLE "Renault Sport: The Game"
#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 720
#define RENDER_WIDTH  240 /// = 960 / 4
#define RENDER_HEIGHT 180 /// = 720 / 4


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