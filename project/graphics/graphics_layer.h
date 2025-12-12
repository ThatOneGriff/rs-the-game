#pragma once
#ifndef GRAPHICS_LAYER_H
#define GRAPHICS_LAYER_H

#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#define WINDOW_TITLE "Joker Demo"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600


/* Struct */

/// Singleton, => static.
struct Graphics_Layer
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  null_texture;
};
static struct Graphics_Layer graphics_layer;
void _init_graphics_layer(void)
{
    graphics_layer.window   = NULL;
    graphics_layer.renderer = NULL;
    graphics_layer.null_texture = NULL;
}

#endif /// GRAPHICS_LAYER_H