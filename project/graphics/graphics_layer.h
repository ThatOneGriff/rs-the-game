#pragma once
#ifndef GRAPHICS_LAYER_H
#define GRAPHICS_LAYER_H

#define WINDOW_TITLE "Renault Sport: The Game"
#define WINDOW_WIDTH  720
#define WINDOW_HEIGHT 960


/* Predef */

struct Graphics_Layer;
void _init_graphics_layer(void);


/* Struct */

struct Graphics_Layer
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  null_texture;
};
static struct Graphics_Layer graphics_layer; /// Singleton.
void _init_graphics_layer(void)
{
    graphics_layer.window   = NULL;
    graphics_layer.renderer = NULL;
    graphics_layer.null_texture = NULL;
}

#endif /// GRAPHICS_LAYER_H