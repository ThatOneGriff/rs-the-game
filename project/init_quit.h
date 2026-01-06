#pragma once
#ifndef INIT_QUIT_H
#define INIT_QUIT_H

#include <SDL3/SDL.h>             /// Initialization
#include <SDL3_image/SDL_image.h> /// of
#include <SDL3_ttf/SDL_ttf.h>     /// SDL3.

#include <stdio.h>  /// I/O

#include "debug.h"     /// Error / warning output.
#include "resources.h" /// Texture & font paths.
#ifdef USING_AUDIO
#include "audio/audio.h"             /// Initialization
#endif /// USING_AUDIO
#include "logic/logic_layer.h"       /// of
#include "graphics/fps.h"            /// various
#include "graphics/graphics_layer.h" /// game
#include "text/text.h"               /// components.

#define SDL_FLAGS (SDL_INIT_VIDEO)


/* Predef */

void init(int* exit_code);
void quit(void);
void program_exit(const int exit_code);


/* Body */

void init(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`init()`: `*exit_code` arg is `NULL`", NON_SDL_ERROR);

    /// SDL3 initialization
    if (! SDL_Init(SDL_FLAGS))
    {
        print_error("`init()`: failed SDL3 initialization", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Graphics layer
    _init_graphics_layer(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: failed to init `graphics_layer`", NON_SDL_ERROR);
        return;
    }
    _init_logic_layer(exit_code);
    if (*exit_code == EXIT_FAILURE) /// NOTE: never happens for now.
    {
        print_error("`init()`: failed to init `logic_layer`", NON_SDL_ERROR);
        return;
    }
    set_fps_cap(60);

    /// TTF initialization
    if (! TTF_Init())
    {
        print_error("`init()`: failed to initialize TTF", IS_SDL_ERROR);
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
        *exit_code = EXIT_FAILURE;
        return;
    }

    _load_global_resources(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: failed to load global resources", NON_SDL_ERROR);
        TTF_Quit();
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Font (test loading)
    TTF_Font* test_main_font_load = TTF_OpenFont(MAIN_FONT_PATH, 1);
    if (test_main_font_load == NULL)
    {
        print_error("`init()`: failed to load the main font", IS_SDL_ERROR);
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

    SDL_SetWindowIcon(graphics_layer.window, ICON_TEXTURE);

    #ifdef USING_AUDIO
    /// Audio
    if (ma_engine_init(NULL, &audio.engine) != MA_SUCCESS)
    {
        print_error("`init()`: failed to initialize audio", NON_SDL_ERROR);
        SDL_DestroyWindow(graphics_layer.window);
        graphics_layer.window = NULL;
        SDL_DestroyRenderer(graphics_layer.renderer);
        graphics_layer.renderer = NULL;
        TTF_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }
    #endif /// USING_AUDIO

    print_success("`init()`");
    *exit_code = EXIT_SUCCESS;
}


/// Works by FILO principle. IDEA: de-init with a stack? Could greatly shorten the code.
void quit(void)
{
    _free_graphics_layer();
    _free_logic_layer();

    TTF_Quit();
    SDL_Quit();
    #ifdef USING_AUDIO
    //ma_sound_uninit (&audio.bg_music); /// Sounds will be added.
    ma_engine_uninit(&audio.engine);
    #endif /// USING_AUDIO
    print_success("`quit()`");
}


void program_exit(const int exit_code)
{
    quit();
    printf("\n");
    exit(exit_code);
}

#endif /// INIT_QUIT_H