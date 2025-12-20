#pragma once
#ifndef INIT_QUIT_H
#define INIT_QUIT_H

#include <SDL3/SDL.h>             /// Initialization
#include <SDL3_image/SDL_image.h> /// of
#include <SDL3_ttf/SDL_ttf.h>     /// SDL3.

#include <stdio.h>  /// I/O

#include "debug.h"  /// Error / warning output.
#include "audio/audio.h"             /// Initialization
#include "logic/logic_layer.h"       /// of
#include "graphics/fps.h"            /// various
#include "graphics/graphics_layer.h" /// game
#include "text/text.h"               /// components.
//#include "game_components/scene.h"

#define SDL_FLAGS (SDL_INIT_VIDEO)


/* Predef */

void init(int* exit_code);
void quit(void);
void program_exit(const int exit_code);


/* Body */


void init(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`init`: `*exit_code` is `NULL`", NON_SDL_ERROR);

    /// Initialization
    if (! SDL_Init(SDL_FLAGS))
    {
        print_error("`init`, initialization", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Window
    graphics_layer.window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (graphics_layer.window == NULL)
    {
        print_error("`init`, window creation", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Renderer
    graphics_layer.renderer = SDL_CreateRenderer(graphics_layer.window, NULL); /// `name` is related to drivers; SDL determines it automatically on `NULL`.
    if (graphics_layer.renderer == NULL)
    {
        print_error("`init`, renderer creation", IS_SDL_ERROR);
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        *exit_code = EXIT_FAILURE;
        return;
    }
    
    /// Null texture (debug purposes)
    graphics_layer.null_texture = IMG_LoadTexture(graphics_layer.renderer, "res/images/null.png");
    if (graphics_layer.null_texture == NULL)
        print_warning("`init`, null texture creation (not critical)", IS_SDL_ERROR);

    /// TTF initialization
    if (! TTF_Init())
    {
        print_error("`init`, TTF initialization", IS_SDL_ERROR);
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Font (test loading)
    TTF_Font* test_main_font_load = TTF_OpenFont(MAIN_FONT, 1);
    if (test_main_font_load == NULL)
    {
        print_error("`init`, main font test loading", IS_SDL_ERROR);
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
        TTF_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }
    TTF_CloseFont(test_main_font_load);
    test_main_font_load = NULL;

    /// Icon
    SDL_Surface* icon = IMG_Load("res/images/icon.png");
    if (icon == NULL)
    {
        print_error("`init`, icon loading", IS_SDL_ERROR);
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
        TTF_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }
    SDL_SetWindowIcon(graphics_layer.window, icon);
    SDL_DestroySurface(icon);
    icon = NULL;

    /// Audio
    if (ma_engine_init(NULL, &audio.engine) != MA_SUCCESS)
    {
        print_error("`init`, audio initialization", NON_SDL_ERROR);
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
        TTF_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }

    set_fps_cap(60);
    print_success("`init`");
    *exit_code = EXIT_SUCCESS;
}


/// Works by FILO principle. IDEA: de-init with a stack? Could greatly shorten the code.
void quit(void)
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
    //ma_sound_uninit (&audio.bg_music);
    ma_engine_uninit(&audio.engine);
    print_success("`quit`");
}


void program_exit(const int exit_code)
{
    quit();
    printf("\n");
    exit(exit_code);
}

#endif /// INIT_QUIT_H