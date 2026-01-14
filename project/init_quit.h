#pragma once
#ifndef INIT_QUIT_H
#define INIT_QUIT_H

/* SDL3 */
#include <SDL3/SDL.h>             /// Initialization
#include <SDL3_image/SDL_image.h> /// of
#include <SDL3_ttf/SDL_ttf.h>     /// SDL3.

/* C headers */
#include <stdio.h> /// I/O.

/* Root folder headers */
#include "debug.h"        /// Error / warning output.
#include "deinit_stack.h" /// Deinitialization stack.
#include "resources.h"    /// Texture & font paths.

/* Audio */
#include "audio/audio_manager.h" /// 'miniaudio' init.
#include "audio/music_loader.h"  /// Music loading.

/* Other headers */
#include "graphics/fps.h"              /// Initialization of
#include "graphics/graphics_layer.h"   /// various
#include "logic/logic_layer.h"         /// game
#include "game_components/text/text.h" /// components.

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
        SDL_Quit();
        return;
    }
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(3, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        SDL_Quit();
        return;
    }
    add_to_deinit_stack(&deinit_stack, graphics_layer.window,   (void (*)(void*))SDL_DestroyWindow);
    add_to_deinit_stack(&deinit_stack, graphics_layer.renderer, (void (*)(void*))SDL_DestroyRenderer);
    add_to_deinit_stack(&deinit_stack, graphics_layer.buffer,   (void (*)(void*))SDL_DestroyTexture);

    /// Logic layer
    _init_logic_layer(exit_code);
    if (*exit_code == EXIT_FAILURE) /// NOTE: never happens for now.
    {
        print_error("`init()`: failed to init `logic_layer`", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        return;
    }
    set_fps_cap(60);

    /// TTF initialization
    if (! TTF_Init())
    {
        print_error("`init()`: failed to initialize TTF", IS_SDL_ERROR);
        _free_logic_layer();
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }

    _load_global_resources(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: failed to load global resources", NON_SDL_ERROR);
        TTF_Quit();
        _free_logic_layer();
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }
    SDL_SetWindowIcon(graphics_layer.window, ICON_TEXTURE);

    /// Music loader
    _init_music_loader("./rsdt/music.rsdt", exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: failed to initialize music loader", NON_SDL_ERROR);
        //_free_music_loader(); /// TODO: enable once available.
        _free_global_resources();
        TTF_Quit();
        _free_logic_layer();
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Audio system
    if (ma_engine_init(NULL, &audio_manager.engine) != MA_SUCCESS)
    {
        print_error("`init()`: failed to initialize audio engine", NON_SDL_ERROR);
        //_free_music_loader(); /// TODO: enable once available.
        _free_global_resources();
        TTF_Quit();
        _free_logic_layer();
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }

    free_deinit_stack(&deinit_stack); /// `free` because those resources will be used.
    print_success("`init()`");
    *exit_code = EXIT_SUCCESS;
}


/// Works by FILO principle.
void quit(void)
{
    //_free_music_loader(); /// TODO: enable once available.
    ma_sound_uninit (&audio_manager.track);
    ma_engine_uninit(&audio_manager.engine);
    _free_global_resources();
    TTF_Quit();
    _free_logic_layer();
    _free_graphics_layer();
    SDL_Quit();

    print_success("`quit()`");
    return;
}


[[ noreturn ]]
void program_exit(const int exit_code)
{
    quit();
    printf("\n");
    exit(exit_code);
}

#endif /// INIT_QUIT_H