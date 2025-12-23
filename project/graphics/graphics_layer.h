#pragma once
#ifndef GRAPHICS_LAYER_H
#define GRAPHICS_LAYER_H

#include <SDL3/SDL.h> /// SDL3.

#define WINDOW_TITLE "Renault Sport: The Game"
#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 720
#define RENDER_WIDTH  240 /// = 960 / 4
#define RENDER_HEIGHT 180 /// = 720 / 4


/* Predef */

struct Graphics_Layer;
static struct Graphics_Layer graphics_layer; /// Singleton.
void _init_graphics_layer(int* exit_code);
void _free_graphics_layer(void);

float center_x(const float w);
float center_y(const float h);


/* Struct */

struct Graphics_Layer
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  null_texture;
    SDL_Texture*  buffer;
};

/// [!!!] Assumes SDL has been initialized.
/// Doesn't check for accidental double initialization.
void _init_graphics_layer(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`_init_graphics_layer()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /* Window */
    graphics_layer.window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (graphics_layer.window == NULL)
    {
        print_error("`_init_graphics_layer()`: failed to create a window", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /* Renderer */
    graphics_layer.renderer = SDL_CreateRenderer(graphics_layer.window, NULL); /// `name` is related to drivers; SDL determines it automatically on `NULL`.
    if (graphics_layer.renderer == NULL)
    {
        print_error("`_init_graphics_layer()`: failed to create a renderer", IS_SDL_ERROR);
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        *exit_code = EXIT_FAILURE;
        return;
    }
    
    /* Null texture (debug purposes) */
    graphics_layer.null_texture = IMG_LoadTexture(graphics_layer.renderer, NULL_TEXTURE);
    if (graphics_layer.null_texture == NULL)
        print_warning("`_init_graphics_layer()`: failed to load the null texture (not critical)", IS_SDL_ERROR);
    
    /* Buffer (for scaled rendering) */
    graphics_layer.buffer = SDL_CreateTexture(graphics_layer.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
    if (graphics_layer.buffer == NULL)
    {
        print_error("`_init_graphics_layer()`: failed to create rendering buffer texture", IS_SDL_ERROR);
        if (graphics_layer.null_texture != NULL)
        {
            SDL_DestroyTexture(graphics_layer.null_texture);
            graphics_layer.null_texture = NULL;
        }
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        *exit_code = EXIT_FAILURE;
        return;
    }
    SDL_SetTextureScaleMode(graphics_layer.buffer, SDL_SCALEMODE_NEAREST);

    *exit_code = EXIT_SUCCESS;
    return;
}

void _free_graphics_layer(void)
{
    if (graphics_layer.renderer != NULL)
    {
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
    }
    if (graphics_layer.window != NULL)
    {
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
    }
    if (graphics_layer.null_texture != NULL)
    {
        SDL_DestroyTexture(graphics_layer.null_texture);
        graphics_layer.null_texture = NULL;
    }
    if (graphics_layer.buffer != NULL)
    {
        SDL_DestroyTexture(graphics_layer.buffer);
        graphics_layer.buffer = NULL;
    }
}


/* Body */

float center_x(const float w)
{
    return (RENDER_WIDTH - w) / 2.0;
}

float center_y(const float h)
{
    return (RENDER_HEIGHT - h) / 2.0;
}

#endif /// GRAPHICS_LAYER_H