/* Related header */
#include "options_screen.h"

#include <SDL3/SDL.h>                        /// SDL3.
#include <stdbool.h>                         /// `bool is_open`.
#include <string.h>                          /// `memset()`.
#include "../../debug.h"                     /// Error message printing.
#include "../../deinit_stack.h"              /// Deinitialization stack.
#include "../../audio/audio_manager.h"       /// `audio_manager.using_audio`.
#include "../../game_components/text/text.h" /// Text.
#include "../../game_components/ui/button.h" /// Buttons.
#include "../../game_components/ui/switch.h" /// Switches.
#include "../../graphics/fps.h"              /// FPS limit switch.
#include "../../graphics/graphics_layer.h"   /// Rendering.

/// NOTE: not my proudest code.
/// I would've shortened a lot of things, if not for the time constraints.


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
    struct Options_Screen result = {0};
    
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(13, exit_code);
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
    result.options_text = create_text("Options", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(X_AUTO_CENTER, 10), 20, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Options' text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.options_text, (void (*)(void*))free_texture);

    /// 'X' button
    result.close_button = create_button("Close", (SDL_Color){237,63,39,255}, vec2(180, 14), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'X' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.close_button, (void (*)(void*))free_button);

    /// 'Audio' text
    result.audio_text = create_text("Audio:", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 50), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Audio:' text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.audio_text, (void (*)(void*))free_texture);

    /// Audio 'ON' button
    struct Button audio_on_button = create_button("ON", (SDL_Color){22,196,127,255}, vec2(70, 50), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the audio 'ON' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &audio_on_button, (void (*)(void*))free_button);
    /// Audio 'OFF' button
    struct Button audio_off_button = create_button("OFF", (SDL_Color){237,63,39,255}, vec2(70, 50), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the audio 'OFF' button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &audio_off_button, (void (*)(void*))free_button);
    /// Audio switch
    result.audio_switch = init_switch(2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the audio switch", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_switch(&result.audio_switch, audio_on_button);
    add_to_switch(&result.audio_switch, audio_off_button);
    result.audio_switch.is_focused = true;
    if (! audio_manager.using_audio)
        change_switch_option(&result.audio_switch);
    add_to_deinit_stack(&deinit_stack, &result.audio_switch, (void (*)(void*))free_switch);

    /// 'FPS limit' text
    result.fps_text = create_text("FPS limit:", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 70), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'FPS limit:' text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.fps_text, (void (*)(void*))free_texture);
    /// FPS limit (30) button
    struct Button fps_button_30 = create_button("30", (SDL_Color){246,255,153,255}, vec2(100, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS limit (30) button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &fps_button_30, (void (*)(void*))free_button);
    /// FPS limit (60) button
    struct Button fps_button_60 = create_button("60", (SDL_Color){22,196,127,255}, vec2(100, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS limit (60) button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &fps_button_60, (void (*)(void*))free_button);
    /// FPS limit (120) button
    struct Button fps_button_120 = create_button("120", (SDL_Color){22,196,127,255}, vec2(100, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS limit (120) button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &fps_button_120, (void (*)(void*))free_button);
    /// FPS limit (none) button
    struct Button fps_button_none = create_button("None", (SDL_Color){69,71,75,255}, vec2(100, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS limit (120) button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &fps_button_none, (void (*)(void*))free_button);
    /// FPS switch
    result.fps_switch = init_switch(4, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS switch", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_switch(&result.fps_switch, fps_button_30);
    add_to_switch(&result.fps_switch, fps_button_60);
    add_to_switch(&result.fps_switch, fps_button_120);
    add_to_switch(&result.fps_switch, fps_button_none);
    for (unsigned int i = 0; i < curr_fps_cap_i; i++)
        change_switch_option(&result.fps_switch); /// Set to curr FPS cap.
    add_to_deinit_stack(&deinit_stack, &result.fps_switch, (void (*)(void*))free_switch);
    
    /// 'Version' text
    result.version_text = create_text("RS The Game v 1.0", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(X_AUTO_CENTER, 150), 9, 1, exit_code);
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

    if (target->last_menu_frame != NULL)
        SDL_DestroyTexture(target->last_menu_frame);

    free_texture(&target->options_text);
    free_button (&target->close_button);

    free_texture(&target->audio_text);
    free_switch (&target->audio_switch);
    
    free_texture(&target->fps_text);
    free_switch (&target->fps_switch);

    free_texture(&target->version_text);

    memset(target, 0, sizeof *target);
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

    /// Preparing background texture.
    SDL_SetRenderTarget(graphics_layer.renderer, target->last_menu_frame);
    SDL_RenderTexture  (graphics_layer.renderer, graphics_layer.buffer, NULL, NULL);
    SDL_SetRenderDrawBlendMode(graphics_layer.renderer, SDL_BLENDMODE_BLEND); /// Allows for semi-transparent layer rendering.

    SDL_SetRenderDrawColor(graphics_layer.renderer, 0, 0, 0, 220);
    SDL_RenderFillRect(graphics_layer.renderer, NULL);

    SDL_SetRenderDrawBlendMode(graphics_layer.renderer, SDL_BLENDMODE_NONE); /// Allows for semi-transparent layer rendering.
    SDL_SetRenderTarget(graphics_layer.renderer, graphics_layer.buffer);

    /// Setting correct button focus.
    target->close_button.is_focused = false;
    target->audio_switch.is_focused = true;
    target->fps_switch.  is_focused = false;
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
    render_texture(&target->options_text);
    render_button (&target->close_button);

    render_texture(&target->audio_text);
    render_switch (&target->audio_switch);

    render_texture(&target->fps_text);
    render_switch (&target->fps_switch);

    render_texture(&target->version_text);
    return;
}