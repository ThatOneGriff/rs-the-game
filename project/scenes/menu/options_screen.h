#pragma once
#ifndef OPTIONS_SCREEN_H
#define OPTIONS_SCREEN_H

#include <SDL3/SDL.h>                        /// SDL3.
#include <stdbool.h>                         /// `bool is_open`.
#include "../../debug.h"                     /// Error message printing.
#include "../../deinit_stack.h"              /// Deinitialization stack.
#include "../../audio/audio_manager.h"       /// `audio_manager.using_audio`.
#include "../../game_components/text/text.h" /// Text.
#include "../../game_components/button.h"    /// Buttons.
#include "../../graphics/graphics_layer.h"   /// Rendering.


/* Struct */

struct Options_Screen
{
    bool is_open;
    SDL_Texture* last_menu_frame;
    SDL_Surface* tint;

    struct Texture options_text;
    struct Button  x_button;

    struct Texture audio_text;
    struct Button  audio_on_off_button;

    struct Texture version_text;
};


/* Predef */

struct Options_Screen init_options_screen(int* exit_code);
void                  free_options_screen(struct Options_Screen* target);
void show_options_screen  (struct Options_Screen* target);
void hide_options_screen  (struct Options_Screen* target);
void render_options_screen(struct Options_Screen* target);


/* Body */

struct Options_Screen init_options_screen(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`init_options_screen()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);

    /// Preparing the object
    struct Options_Screen result;
    result.is_open         = false;
    result.last_menu_frame = NULL;
    result.tint            = NULL;
    
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(6, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create deinit stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return result;
    }

    result.last_menu_frame = SDL_CreateTexture(graphics_layer.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
    if (result.last_menu_frame == NULL)
    {
        print_error("`init_options_screen()`: couldn't create the last menu frame texture", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, result.last_menu_frame, (void (*)(void*))SDL_DestroyTexture);

    /// 'Options' text
    result.options_text = create_text("Options", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(X_AUTO_CENTER, 10), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Options' text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.options_text, (void (*)(void*))free_texture);

    /// 'X' button
    result.x_button = create_button("X", (SDL_Color){237,63,39,255}, vec2(140, 50), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'X' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.x_button, (void (*)(void*))free_button);

    /// 'Audio' text
    result.audio_text = create_text("Audio:", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 50), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Audio:' text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.audio_text, (void (*)(void*))free_texture);

    /// Audio 'ON/OFF' button
    result.audio_on_off_button = create_button("Audio:", (SDL_Color){255,255,255,255}, vec2(70, 50), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the audio 'ON/OFF' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    result.audio_on_off_button.is_focused = true;
    add_to_deinit_stack(&deinit_stack, &result.audio_on_off_button, (void (*)(void*))free_button);
    
    /// 'Version' text
    result.version_text = create_text("RS The Game ver. 0.0.1", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(X_AUTO_CENTER, 150), 9, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the version text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }

    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_options_screen(struct Options_Screen* target)
{
    if (target == NULL)
    {
        print_error("`free_options_screen()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    target->is_open = false;
    if (target->last_menu_frame != NULL)
    {
        SDL_DestroyTexture(target->last_menu_frame); /// UNTESTED and may corrupt the rendering buffer.
        target->last_menu_frame = NULL;
    }
    if (target->tint != NULL)
    {
        SDL_DestroySurface(target->tint); /// UNTESTED and may corrupt the rendering buffer.
        target->tint = NULL;
    }

    free_texture(&target->options_text);
    free_button (&target->x_button);

    free_texture(&target->audio_text);
    free_button (&target->audio_on_off_button);

    free_texture(&target->version_text);
    return;
}


void show_options_screen(struct Options_Screen* target)
{
    if (target == NULL)
    {
        print_error("`show_options_screen()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    target->is_open = true;
    SDL_SetRenderTarget(graphics_layer.renderer, target->last_menu_frame);
    SDL_RenderTexture  (graphics_layer.renderer, graphics_layer.buffer, NULL, NULL);
    SDL_SetRenderDrawBlendMode(graphics_layer.renderer, SDL_BLENDMODE_BLEND); /// Allows for semi-transparent layer rendering.

    SDL_SetRenderDrawColor(graphics_layer.renderer, 0, 0, 0, 127);
    SDL_RenderFillRect(graphics_layer.renderer, NULL);

    SDL_SetRenderDrawBlendMode(graphics_layer.renderer, SDL_BLENDMODE_NONE); /// Allows for semi-transparent layer rendering.
    SDL_SetRenderTarget(graphics_layer.renderer, graphics_layer.buffer);
    return;
}


void hide_options_screen(struct Options_Screen* target)
{
    if (target == NULL)
    {
        print_error("`hide_options_screen()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    target->is_open = false;
    if (target->last_menu_frame != NULL)
    {
        SDL_DestroyTexture(target->last_menu_frame); /// UNTESTED and may corrupt the rendering buffer.
        target->last_menu_frame = NULL;
    }
    return;
}


void render_options_screen(struct Options_Screen* target)
{
    if (target == NULL)
    {
        print_error("`hide_options_screen()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    SDL_RenderTexture(graphics_layer.renderer, target->last_menu_frame, NULL, NULL);

    /// <...>

    return;
}

#endif /// OPTIONS_SCREEN_H