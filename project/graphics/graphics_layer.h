#pragma once
#ifndef GRAPHICS_LAYER_H
#define GRAPHICS_LAYER_H

/* SDL3 */
#include <SDL3/SDL.h> /// SDL things.

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
};
extern struct Graphics_Layer graphics_layer; /// Singleton.


/* Predef */

void _init_graphics_layer(int* exit_code);
void _free_graphics_layer(void);


#endif /// GRAPHICS_LAYER_H