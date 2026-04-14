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
#include "../../game_components/ui/pseudo_button.h" /// Pseudo buttons.
#include "../../game_components/ui/switch.h" /// Switches.
#include "../../graphics/fps.h"              /// FPS limit switch.
#include "../../graphics/graphics_layer.h"   /// Rendering.


/* Predef */

struct Options_Screen init_options_screen(int *const exit_code);
void                  free_options_screen(struct Options_Screen *const target);
void show_options_screen  (struct Options_Screen *const target);
void hide_options_screen  (struct Options_Screen *const target);
void render_options_screen(struct Options_Screen *const target);


/* Body */

struct Options_Screen init_options_screen(int *const exit_code)
{
    if (exit_code == NULL)
        print_warning("`init_options_screen()`: `exit_code` arg is `NULL`");

    /// Preparing the object
    struct Options_Screen result = {0};
    
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(21, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create deinit stack");
        free_deinit_stack(&deinit_stack);
        return result;
    }

    result.last_menu_frame = SDL_CreateTexture(graphics_layer.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
    if (result.last_menu_frame == NULL)
    {
        print_error("`init_options_screen()`: couldn't create the last menu frame texture");
        free_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, result.last_menu_frame, (void (*)(void*))SDL_DestroyTexture);

    /// 'Options' text
    result.options_text = create_text("Options", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(X_AUTO_CENTER, 10), 20, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Options' text");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.options_text, (void (*)(void*))free_sprite);

    /// 'X' button
    result.close_button = create_button("Close", (SDL_Color){237,63,39,255}, vec2(180, 14), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'X' button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.close_button, (void (*)(void*))free_button);

    /// 'Audio' text
    result.audio_text = create_text("Audio:", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 50), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Audio:' text");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.audio_text, (void (*)(void*))free_sprite);
    
    /// Audio 'ON' button
    const SDL_Color AUDIO_ON_SWITCH_COLOR = (audio_manager.audio_is_valid ? (SDL_Color){22,196,127,255} : DISABLED_BUTTON_COLOR);
    struct Pseudo_Button audio_on_button = create_pseudo_button("ON", AUDIO_ON_SWITCH_COLOR, vec2(65, 50), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the audio 'ON' pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &audio_on_button, (void (*)(void*))free_pseudo_button);
    /// Audio 'OFF' button
    const SDL_Color AUDIO_OFF_SWITCH_COLOR = (audio_manager.audio_is_valid ? (SDL_Color){237,63,39,255} : DISABLED_BUTTON_COLOR);
    struct Pseudo_Button audio_off_button = create_pseudo_button("OFF", AUDIO_OFF_SWITCH_COLOR, vec2(65, 50), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the audio 'OFF' pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &audio_off_button, (void (*)(void*))free_pseudo_button);
    /// Audio switch
    result.audio_switch = init_switch(2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the audio switch");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_switch(&result.audio_switch, audio_on_button);
    add_to_switch(&result.audio_switch, audio_off_button);
    if (audio_manager.audio_is_valid)
        result.audio_switch.is_focused = true;
    if (! audio_manager.using_audio || ! audio_manager.audio_is_valid)
        next_switch_option(&result.audio_switch);
    add_to_deinit_stack(&deinit_stack, &result.audio_switch, (void (*)(void*))free_switch);

    /// 'FPS limit' text
    result.fps_text = create_text("Gameplay FPS limit:", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 70), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'FPS limit:' text");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.fps_text, (void (*)(void*))free_sprite);
    /// FPS limit (30) button
    struct Pseudo_Button fps_button_30 = create_pseudo_button("30", (SDL_Color){246,255,153,255}, vec2(189, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS limit (30) pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &fps_button_30, (void (*)(void*))free_pseudo_button);
    /// FPS limit (60) button
    struct Pseudo_Button fps_button_60 = create_pseudo_button("60", (SDL_Color){22,196,127,255}, vec2(189, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS limit (60) pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &fps_button_60, (void (*)(void*))free_pseudo_button);
    /// FPS limit (120) button
    struct Pseudo_Button fps_button_120 = create_pseudo_button("120", (SDL_Color){22,196,127,255}, vec2(189, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS limit (120) pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &fps_button_120, (void (*)(void*))free_pseudo_button);
    /// FPS limit (none) button
    struct Pseudo_Button fps_button_none = create_pseudo_button("None", (SDL_Color){69,71,75,255}, vec2(189, 70), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS limit ('None') pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &fps_button_none, (void (*)(void*))free_pseudo_button);
    /// FPS switch
    result.fps_switch = init_switch(4, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the FPS switch");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_switch(&result.fps_switch, fps_button_30);
    add_to_switch(&result.fps_switch, fps_button_60);
    add_to_switch(&result.fps_switch, fps_button_120);
    add_to_switch(&result.fps_switch, fps_button_none);
    if (! audio_manager.audio_is_valid)
        result.fps_switch.is_focused = true;
    for (unsigned int i = 0; i < curr_fps_cap_i; i++)
        next_switch_option(&result.fps_switch); /// Set to curr FPS cap.
    add_to_deinit_stack(&deinit_stack, &result.fps_switch, (void (*)(void*))free_switch);

    /// 'Show FPS' text
    result.show_fps_text = create_text("Show FPS:", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 90), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Show FPS:' text");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.show_fps_text, (void (*)(void*))free_sprite);

    /// Show FPS 'ON' button
    struct Pseudo_Button show_fps_on_button = create_pseudo_button("ON", (SDL_Color){22,196,127,255}, vec2(102, 90), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Show FPS/ON' pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &show_fps_on_button, (void (*)(void*))free_pseudo_button);
    /// Show FPS 'OFF' button
    struct Pseudo_Button show_fps_off_button = create_pseudo_button("OFF", (SDL_Color){237,63,39,255}, vec2(102, 90), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Show FPS/OFF' pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &show_fps_off_button, (void (*)(void*))free_pseudo_button);
    /// 'Show FPS' switch
    result.show_fps_switch = init_switch(2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Show FPS' switch");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_switch(&result.show_fps_switch, show_fps_on_button );
    add_to_switch(&result.show_fps_switch, show_fps_off_button);
    if (! show_fps)
        next_switch_option(&result.show_fps_switch);
    add_to_deinit_stack(&deinit_stack, &result.show_fps_switch, (void (*)(void*))free_switch);

    /// 'Logging' text
    result.logging_text = create_text("Logging:", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 110), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the 'Logging:' text");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.logging_text, (void (*)(void*))free_sprite);
    /// Logging 'ON' button
    struct Pseudo_Button logging_on_button = create_pseudo_button("ON", (SDL_Color){22,196,127,255}, vec2(82, 110), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the logging 'ON' pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &logging_on_button, (void (*)(void*))free_pseudo_button);
    /// Logging 'OFF' button
    struct Pseudo_Button logging_off_button = create_pseudo_button("OFF", (SDL_Color){237,63,39,255}, vec2(82, 110), 15, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the logging 'OFF' pseudo-button");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &logging_off_button, (void (*)(void*))free_pseudo_button);
    /// 'Logging' switch
    result.logging_switch = init_switch(2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the logging switch");
        flush_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_switch(&result.logging_switch, logging_on_button );
    add_to_switch(&result.logging_switch, logging_off_button);
    if (! logic_layer.logging_enabled)
        next_switch_option(&result.logging_switch);
    add_to_deinit_stack(&deinit_stack, &result.logging_switch, (void (*)(void*))free_switch);
    
    /// 'Version' text
    result.version_text = create_text("RS The Game v 1.2", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(X_AUTO_CENTER, 150), 9, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init_options_screen()`: couldn't create the version text");
        flush_deinit_stack(&deinit_stack);
        return result;
    }

    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_options_screen(struct Options_Screen *const target)
{
    if (target == NULL)
    {
        print_error("`free_options_screen()`: `target` arg is `NULL`");
        return;
    }

    if (target->last_menu_frame != NULL)
        SDL_DestroyTexture(target->last_menu_frame);

    free_sprite(&target->options_text);
    free_button(&target->close_button);

    free_sprite(&target->audio_text);
    free_switch(&target->audio_switch);
    
    free_sprite(&target->fps_text);
    free_switch(&target->fps_switch);

    free_sprite(&target->show_fps_text);
    free_switch(&target->show_fps_switch);

    free_sprite(&target->logging_text);
    free_switch(&target->logging_switch);

    free_sprite(&target->version_text);

    memset(target, 0, sizeof *target);
    return;
}


void show_options_screen(struct Options_Screen *const target)
{
    if (target == NULL)
    {
        print_error("`show_options_screen()`: `target` arg is `NULL`");
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
    target->close_button.is_focused =   false;
    target->audio_switch.is_focused =   audio_manager.audio_is_valid;
    target->fps_switch.  is_focused = ! audio_manager.audio_is_valid;
    target->curr_focus = &target->audio_switch;
    
    graphics_layer.force_render = true;
    return;
}


void hide_options_screen(struct Options_Screen *const target)
{
    if (target == NULL)
    {
        print_error("`hide_options_screen()`: `target` arg is `NULL`");
        return;
    }

    target->is_open = false;
    return;
}


void render_options_screen(struct Options_Screen *const target)
{
    if (target == NULL)
    {
        print_error("`hide_options_screen()`: `target` arg is `NULL`");
        return;
    }

    SDL_RenderTexture(graphics_layer.renderer, target->last_menu_frame, NULL, NULL);
    render_sprite(&target->options_text);
    render_button(&target->close_button);

    render_sprite(&target->audio_text);
    render_switch(&target->audio_switch);

    render_sprite(&target->fps_text);
    render_switch(&target->fps_switch);

    render_sprite(&target->show_fps_text);
    render_switch(&target->show_fps_switch);

    render_sprite(&target->logging_text);
    render_switch(&target->logging_switch);

    render_sprite(&target->version_text);
    return;
}