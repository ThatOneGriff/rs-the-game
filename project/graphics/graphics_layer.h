#pragma once
#ifndef GRAPHICS_LAYER_H
#define GRAPHICS_LAYER_H

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* Helper headers */
#include "../debug.h"        /// Error printing.
#include "../deinit_stack.h" /// Deinitialization stack.

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


/* Struct */

struct Graphics_Layer
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  buffer;
};

/// [!!!] Assumes SDL has been initialized.
/// Doesn't check for accidental double initialization.
void _init_graphics_layer(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`_init_graphics_layer()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(2, exit_code); /// Not adding the last element (font loading). Also, `global_data` needs its own treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    
    /// Window
    graphics_layer.window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (graphics_layer.window == NULL)
    {
        free_deinit_stack(&deinit_stack);
        print_error("`_init_graphics_layer()`: failed to create a window", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, graphics_layer.window, (void (*)(void*))SDL_DestroyWindow);

    /// Renderer
    graphics_layer.renderer = SDL_CreateRenderer(graphics_layer.window, NULL); /// `name` is related to drivers; SDL determines it automatically on `NULL`.
    if (graphics_layer.renderer == NULL)
    {
        print_error("`_init_graphics_layer()`: failed to create a renderer", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, graphics_layer.renderer, (void (*)(void*))SDL_DestroyRenderer);
    
    /// Buffer texture (for scaled rendering)
    graphics_layer.buffer = SDL_CreateTexture(graphics_layer.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
    if (graphics_layer.buffer == NULL)
    {
        print_error("`_init_graphics_layer()`: failed to create rendering buffer texture", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return;
    }
    SDL_SetTextureScaleMode(graphics_layer.buffer, SDL_SCALEMODE_NEAREST);
    
    free_deinit_stack(&deinit_stack);
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
    if (graphics_layer.buffer != NULL)
    {
        SDL_DestroyTexture(graphics_layer.buffer);
        graphics_layer.buffer = NULL;
    }
}

#endif /// GRAPHICS_LAYER_H