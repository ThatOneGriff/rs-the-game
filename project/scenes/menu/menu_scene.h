#pragma once
#ifndef MENU_SCENE_H
#define MENU_SCENE_H

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* C headers */
#include <stdlib.h>  /// `malloc()`
#include <stdbool.h> /// Setting button focuses.

/* Helper headers */
#include "../../debug.h"           /// Error printing.
#include "../../deinit_stack.h"    /// Deinitialization stack.
#include "../../helpers/helpers.h" /// `free_ptr_arr()`.
#include "../../resources.h"       /// Null texture.

/* Graphics and components */
#include "options_screen.h"                  /// Options screen.
#include "../../game_components/button.h"    /// Button.
#include "../../game_components/texture.h"   /// Texture.
#include "../../game_components/text/text.h" /// Text creation.
#include "../../graphics/graphics_layer.h"   /// `graphics_layer`.

#define MENU_DATA_PATH "./rsdt/scene_data/menu.rsdt" /// Unlike `Gameplay_Scene`'s, there's only one concrete file for the menu.
#define MENU_DATA_LINES 1


/* Struct */

struct Menu_Scene
{
    SDL_Texture* bg;
    struct Texture car_name_text;
    struct Button next_button;
    struct Button prev_button;

    struct Button play_button;
    struct Button         options_button;
    struct Options_Screen options_screen;
    struct Button quit_button;

    struct Texture photo_quad1;
    struct Texture photo_quad2;
    struct Texture photo_quad3;
    struct Texture photo_quad4;
    
    struct Texture year_text;
    struct Texture horsepower_text;
    struct Texture top_speed_text;
    struct Texture handling_text;
    
    struct Texture info_line1;
    struct Texture info_line2;
};


/* Predef */

struct Menu_Scene load_menu_scene(int* exit_code);
void              free_menu_scene(struct Menu_Scene* target);
void              render_menu_scene(struct Menu_Scene* target);


/* Body */

struct Menu_Scene load_menu_scene(int* exit_code)
{
    struct Menu_Scene result;
    result.bg = NULL;
    //result.button_focus_i = 0;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`load_menu_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Reading the file
    char** scene_data = read_file_by_line(MENU_DATA_PATH, MENU_DATA_LINES);
    if (scene_data == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't read data from file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Background
    result.bg = IMG_LoadTexture(graphics_layer.renderer, scene_data[0]);
    if (result.bg == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`load_menu_scene()`: couldn't load the bg texture, replaced by null texture", IS_SDL_ERROR);
            result.bg = NULL_TEXTURE;
        }
        else
        {
            print_error("`load_menu_scene()`: couldn't load the bg texture, and null texture is empty", IS_SDL_ERROR);
            free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(8, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    if (result.bg != NULL_TEXTURE)
        add_to_deinit_stack(&deinit_stack, result.bg, (void (*)(void*))SDL_DestroyTexture);

    /// Car name text
    result.car_name_text = create_text("Clio Williams", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 10), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create the text", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.car_name_text, (void (*)(void*))free_texture);

    /// Next button
    result.next_button = create_button("NEXT", (SDL_Color){69,71,75,255}, vec2(155, 15), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create next button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.next_button, (void (*)(void*))free_button);

    /// Prev button
    result.prev_button = create_button("PREV", (SDL_Color){69,71,75,255}, vec2(195, 15), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create prev button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.prev_button, (void (*)(void*))free_button);

    /// Play button
    result.play_button = create_button("PLAY", (SDL_Color){254,178,26,255}, vec2(155, 82), 25, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create play button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    result.play_button.is_focused = true;
    add_to_deinit_stack(&deinit_stack, &result.play_button, (void (*)(void*))free_button);

    /// Options button
    result.options_button = create_button("OPTIONS", (SDL_Color){19,70,134,255}, vec2(155, 107), 18, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create options button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.options_button, (void (*)(void*))free_button);

    /// Options screen
    result.options_screen = init_options_screen(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't init options screen", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.options_screen, (void (*)(void*))free_options_screen);

    /// Quit button
    result.quit_button = create_button("QUIT", (SDL_Color){237,63,39,255}, vec2(155, 125), 18, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create quit button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.quit_button, (void (*)(void*))free_button);

    /// Photo quads. REDO: very hard-coded and unsafe as of now.
    result.photo_quad1 = load_texture("./res/images/cars/clio-williams/quad1.png", (SDL_FRect){10   , 34   , 72, 54}, exit_code);
    result.photo_quad2 = load_texture("./res/images/cars/clio-williams/quad2.png", (SDL_FRect){10+72, 34   , 72, 54}, exit_code);
    result.photo_quad3 = load_texture("./res/images/cars/clio-williams/quad3.png", (SDL_FRect){10   , 34+54, 72, 54}, exit_code);
    result.photo_quad4 = load_texture("./res/images/cars/clio-williams/quad4.png", (SDL_FRect){10+72, 34+54, 72, 54}, exit_code);

    result.year_text        = create_text("Year: 1994",    (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 33), 9, 1, exit_code);
    result.horsepower_text  = create_text("Engine: 150 hp", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 43), 9, 1, exit_code);
    result.top_speed_text   = create_text("Speed: 215 kph",    (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 53), 9, 1, exit_code);
    result.handling_text    = create_text("Handling: 80",  (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 63), 9, 1, exit_code);
    
    result.info_line1 = create_text("Honoring the Williams F1 team, this was", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(10, 150), 9, 1, exit_code);
    result.info_line2 = create_text("the first sporty Clio of many to come.",  (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(10, 160), 9, 1, exit_code);

    free_deinit_stack(&deinit_stack);
    free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_menu_scene(struct Menu_Scene* target)
{
    if (target->bg != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->bg);
        target->bg = NULL;
    }
    free_texture(&target->car_name_text);
    free_button(&target->next_button);
    free_button(&target->prev_button);

    free_button(&target->play_button);
    free_button(&target->options_button);
    free_button(&target->quit_button);

    free_texture(&target->photo_quad1);
    free_texture(&target->photo_quad2);
    free_texture(&target->photo_quad3);
    free_texture(&target->photo_quad4);
    
    free_texture(&target->year_text);
    free_texture(&target->horsepower_text);
    free_texture(&target->top_speed_text);
    free_texture(&target->handling_text);
    
    free_texture(&target->info_line1);
    free_texture(&target->info_line2);
}


void render_menu_scene(struct Menu_Scene* target)
{
    if (target == NULL) /// TODO: check all members.
    {
        print_error("`menu_scene_tick()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    if (target->options_screen.is_open)
    {
        render_options_screen(&target->options_screen);
        return;
    }

    SDL_RenderTexture(graphics_layer.renderer, target->bg, NULL, NULL);
    render_texture(&target->car_name_text);
    render_button(&target->next_button);
    render_button(&target->prev_button);

    render_button(&target->play_button);
    render_button(&target->options_button);
    render_button(&target->quit_button);

    render_texture(&target->photo_quad1);
    render_texture(&target->photo_quad2);
    render_texture(&target->photo_quad3);
    render_texture(&target->photo_quad4);
    
    render_texture(&target->year_text);
    render_texture(&target->horsepower_text);
    render_texture(&target->top_speed_text);
    render_texture(&target->handling_text);
    
    render_texture(&target->info_line1);
    render_texture(&target->info_line2);
    return;
}

#endif /// MENU_SCENE_H