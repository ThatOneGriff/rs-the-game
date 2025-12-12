#pragma once
#ifndef INIT_QUIT_H
#define INIT_QUIT_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>

#include "logic_layer.h"
#include "audio/audio.h"
#include "game_components/scene.h"
#include "graphics/fps.h"
#include "graphics/graphics_layer.h"
#include "text/text.h"

#define SDL_FLAGS (SDL_INIT_VIDEO)


/* Predef */

void init_SDL(int* exit_code);
void free_SDL(void);
void program_exit(const int exit_code);


/* Body */


void init_SDL(int* exit_code)
{
    /// Initialization
    if (! SDL_Init(SDL_FLAGS))
    {
        fprintf(stderr, "~ [ERROR] while initializing SDL3: %s\n", SDL_GetError());
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Window
    graphics_layer.window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (graphics_layer.window == NULL)
    {
        fprintf(stderr, "~ [ERROR] while creating Window: %s\n", SDL_GetError());
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Renderer
    graphics_layer.renderer = SDL_CreateRenderer(graphics_layer.window, NULL); /// `name` is related to drivers; SDL determines it automatically on `NULL`.
    if (graphics_layer.renderer == NULL)
    {
        fprintf(stderr, "~ [ERROR] while creating Renderer: %s\n", SDL_GetError());
        *exit_code = EXIT_FAILURE;
        return;
    }
    
    graphics_layer.null_texture = IMG_LoadTexture(graphics_layer.renderer, "res/images/null.png");
    if (graphics_layer.null_texture == NULL)
        fprintf(stderr, "~ [WRNNG] `null_texture` couldn't be loaded; not critical.\n");

    /// TTF initialization
    if (! TTF_Init())
    {
        fprintf(stderr, "~ [ERROR] while initializing TTF: %s\n", SDL_GetError());
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Font
    TTF_Font* test_load_main_font = TTF_OpenFont(MAIN_FONT, 1);
    if (test_load_main_font == NULL)
    {
        fprintf(stderr, "~ [ERROR] while loading main font: %s\n", SDL_GetError());
        *exit_code = EXIT_FAILURE;
        return;
    }
    TTF_CloseFont(test_load_main_font);
    test_load_main_font = NULL;

    /// Icon
    SDL_Surface* icon = IMG_Load("res/images/icon.png");
    if (icon == NULL)
    {
        fprintf(stderr, "~ [ERROR] while loading icon: %s\n", SDL_GetError());
        *exit_code = EXIT_FAILURE;
        return;
    }
    SDL_SetWindowIcon(graphics_layer.window, icon);
    SDL_DestroySurface(icon);
    icon = NULL;

    set_fps_cap(60);
    printf("[SUCCESS] Init\n");
    *exit_code = EXIT_SUCCESS;
}


/// Works by FILO principle. De-init with a stack, maybe?
void free_SDL(void)
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

    //if (logic_layer.key_state != NULL)
    //    logic_layer.key_state  = NULL;

    TTF_Quit();
    SDL_Quit();
    printf("[SUCCESS] Quit\n");
}

void program_exit(const int exit_code)
{
    ma_sound_uninit (&audio.bg_music);
    ma_engine_uninit(&audio.engine);
    free_SDL();
    printf("\n");
    exit(exit_code);
}

#endif /// INIT_QUIT_H