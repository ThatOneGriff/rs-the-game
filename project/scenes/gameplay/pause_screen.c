/* Related header */
#include "pause_screen.h"

#include <SDL3/SDL.h>                        /// SDL3.
#include <SDL3/SDL_timer.h>                  /// Time measurement.
#include <stdbool.h>                         /// `bool is_open`.
#include <stdio.h>                           /// Text debug (TEMP).
#include <string.h>                          /// `memset()`.
#include "../../debug.h"                     /// Error message printing.
#include "../../deinit_stack.h"              /// Deinitialization stack.
#include "../../audio/audio_manager.h"       /// `audio_manager.using_audio`.
#include "../../game_components/text/text.h" /// Text.
#include "../../game_components/ui/button.h" /// Buttons.
#include "../../game_components/ui/switch.h" /// Switches.
#include "../../graphics/graphics_layer.h"   /// Rendering.
#include "../../logic/logic_layer.h"         /// Time tick manipulation.

/// NOTE: not my proudest code.
/// I would've shortened a lot of things, if not for the time constraints.


/* Predef */

struct Pause_Screen init_pause_screen(int *const exit_code);
void                free_pause_screen(struct Pause_Screen *const target);
void show_pause_screen  (struct Pause_Screen *const target);
void hide_pause_screen  (struct Pause_Screen *const target);
void render_pause_screen(struct Pause_Screen *const target);


/* Body */

struct Pause_Screen init_pause_screen(int *const exit_code)
{
    if (exit_code == NULL)
        print_warning("`init_pause_screen()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);

    /// Preparing the object
    struct Pause_Screen result = {0};
    
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(6, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_pause_screen()`: couldn't create deinit stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return result;
    }

    result.last_gameplay_frame = SDL_CreateTexture(graphics_layer.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
    if (result.last_gameplay_frame == NULL)
    {
        print_error("`init_pause_screen()`: couldn't create the last menu frame texture", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, result.last_gameplay_frame, (void (*)(void*))SDL_DestroyTexture);

    /// 'Pause' text
    result.pause_text = create_text("Pause", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(X_AUTO_CENTER, 10), 20, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_pause_screen()`: couldn't create the 'Pause' text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.pause_text, (void (*)(void*))free_texture);

    /// 'X' button
    result.close_button = create_button("Close", (SDL_Color){237,63,39,255}, vec2(180, 14), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_pause_screen()`: couldn't create the 'X' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.close_button, (void (*)(void*))free_button);
    
    /// 'Continue' button
    result.continue_button = create_button("Continue", (SDL_Color){22,196,127,255}, vec2(10, 50), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_pause_screen()`: couldn't create the 'Continue' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    result.continue_button.is_focused = true;
    add_to_deinit_stack(&deinit_stack, &result.continue_button, (void (*)(void*))free_button);

    /// 'Quit to menu' button
    result.quit_to_menu_button = create_button("Quit to menu", (SDL_Color){69,71,75,255}, vec2(10, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_pause_screen()`: couldn't create the 'Quit to menu' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.quit_to_menu_button, (void (*)(void*))free_button);

    /// 'Quit to desktop' button
    result.quit_to_desktop_button = create_button("Quit to desktop", (SDL_Color){69,71,75,255}, vec2(10, 90), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_pause_screen()`: couldn't create the 'Quit to desktop' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.quit_to_desktop_button, (void (*)(void*))free_button);
    
    /// 'Version' text
    result.version_text = create_text("RS The Game v 1.1", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(X_AUTO_CENTER, 150), 9, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_pause_screen()`: couldn't create the version text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }

    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_pause_screen(struct Pause_Screen *const target)
{
    if (target == NULL)
    {
        print_error("`free_pause_screen()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    if (target->last_gameplay_frame != NULL)
        SDL_DestroyTexture(target->last_gameplay_frame);

    free_texture(&target->pause_text);

    free_button (&target->          close_button);
    free_button (&target->       continue_button);
    free_button (&target->   quit_to_menu_button);
    free_button (&target->quit_to_desktop_button);

    free_texture(&target->version_text);

    memset(target, 0, sizeof *target);
    return;
}


void show_pause_screen(struct Pause_Screen *const target)
{
    if (target == NULL)
    {
        print_error("`show_pause_screen()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    target->is_open = true;
    target->open_tick = logic_layer.curr_tick;

    /// Preparing background texture.
    SDL_SetRenderTarget(graphics_layer.renderer, target->last_gameplay_frame);
    SDL_RenderTexture  (graphics_layer.renderer, graphics_layer.buffer, NULL, NULL);
    SDL_SetRenderDrawBlendMode(graphics_layer.renderer, SDL_BLENDMODE_BLEND); /// Allows for semi-transparent layer rendering.

    SDL_SetRenderDrawColor(graphics_layer.renderer, 0, 0, 0, 220);
    SDL_RenderFillRect(graphics_layer.renderer, NULL);

    SDL_SetRenderDrawBlendMode(graphics_layer.renderer, SDL_BLENDMODE_NONE); /// Allows for semi-transparent layer rendering.
    SDL_SetRenderTarget(graphics_layer.renderer, graphics_layer.buffer);

    /// Setting correct button focus.
    target->curr_button->is_focused = false; /// Latest chosen button.
    target->curr_button = &target->continue_button;
    target->curr_button->is_focused = true;

    graphics_layer.force_render = true;
    return;
}


void hide_pause_screen(struct Pause_Screen *const target)
{
    if (target == NULL)
    {
        print_error("`hide_pause_screen()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    target->is_open = false;
    logic_layer.real_tick_diff += logic_layer.curr_tick - target->open_tick; /// Making up for the paused time.
    logic_layer.curr_tick       = SDL_GetTicks() - logic_layer.real_tick_diff;
    target->open_tick     = 0;
    return;
}


void render_pause_screen(struct Pause_Screen *const target)
{
    if (target == NULL)
    {
        print_error("`hide_pause_screen()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    SDL_RenderTexture(graphics_layer.renderer, target->last_gameplay_frame, NULL, NULL);
    render_texture(&target->pause_text);

    render_button          (&target->close_button);
    render_button       (&target->continue_button);
    render_button   (&target->quit_to_menu_button);
    render_button(&target->quit_to_desktop_button);

    render_texture(&target->version_text);
    return;
}