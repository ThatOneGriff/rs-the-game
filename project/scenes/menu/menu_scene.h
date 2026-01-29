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
#include "../car.h"                          /// Car data.
#include "options_screen.h"                  /// Options screen.
#include "../../game_components/ui/button.h"    /// Button.
#include "../../game_components/graphics/texture.h"   /// Texture.
#include "../../game_components/text/text.h" /// Text creation.
#include "../../graphics/graphics_layer.h"   /// `graphics_layer`.

#define MENU_DATA_PATH "./rsdt/scene_data/menu.rsdt" /// Unlike `Gameplay_Scene`'s, there's only one concrete file for the menu.
#define MENU_DATA_LINES 1


/* Struct */

struct Menu_Scene
{
    struct Car* car_ptr;

    SDL_Texture* bg;
    struct Texture car_name_text;
    struct Button prev_button;
    struct Button next_button;

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

struct Menu_Scene load_menu_scene   (struct Car* car_ptr, int* exit_code);
void              free_menu_scene   (struct Menu_Scene* target);
void               set_menu_car_info(struct Menu_Scene* scene, struct Car* car, int* exit_code);
void            render_menu_scene   (struct Menu_Scene* target);


/* Body */

struct Menu_Scene load_menu_scene(struct Car* car_ptr, int* exit_code)
{
    struct Menu_Scene result;
    result.bg = NULL;
    result.car_ptr = NULL;
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
    struct Deinit_Stack deinit_stack = new_deinit_stack(6, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    if (result.bg != NULL_TEXTURE)
        add_to_deinit_stack(&deinit_stack, result.bg, (void (*)(void*))SDL_DestroyTexture);

    /// Prev button
    result.prev_button = create_button("PREV", (SDL_Color){69,71,75,255}, vec2(155, 15), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create prev button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.prev_button, (void (*)(void*))free_button);

    /// Next button
    result.next_button = create_button("NEXT", (SDL_Color){69,71,75,255}, vec2(195, 15), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create next button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.next_button, (void (*)(void*))free_button);

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

    set_menu_car_info(&result, car_ptr, exit_code);

    free_deinit_stack(&deinit_stack);
    free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_menu_scene(struct Menu_Scene* target)
{
    target->car_ptr = NULL;
    if (target->bg != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->bg);
        target->bg = NULL;
    }
    free_texture(&target->car_name_text);
    free_button(&target->prev_button);
    free_button(&target->next_button);

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


void set_menu_car_info(struct Menu_Scene* scene, struct Car* car, int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`set_menu_car_info()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (scene == NULL || car == NULL) /// TODO: check all members.
    {
        print_error("`set_menu_car_info()`: `scene` or `car` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    scene->car_ptr = car;
    /// Car name text
    scene->car_name_text = create_text(car->name, (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 10), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`set_menu_car_info()`: couldn't create the text", NON_SDL_ERROR);
        return;
    }
    
    /// TODO: checks.
    scene->photo_quad1 = load_texture(car->quad_paths[0], (SDL_FRect){10   , 34   , 72, 54}, exit_code);
    scene->photo_quad2 = load_texture(car->quad_paths[1], (SDL_FRect){10+72, 34   , 72, 54}, exit_code);
    scene->photo_quad3 = load_texture(car->quad_paths[2], (SDL_FRect){10   , 34+54, 72, 54}, exit_code);
    scene->photo_quad4 = load_texture(car->quad_paths[3], (SDL_FRect){10+72, 34+54, 72, 54}, exit_code);
    
    char year_text[15];
    sprintf(year_text, "Year: %d", car->year);
    char engine_text[15];
    sprintf(engine_text, "Engine: %d hp", car->hp);
    char speed_text[15];
    sprintf(speed_text, "Speed: %d kph", car->top_speed);
    char handling_text[14];
    sprintf(handling_text, "Handling: %d", car->handling);
    scene->year_text        = create_text(year_text,     (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 33), 9, 1, exit_code);
    scene->horsepower_text  = create_text(engine_text,   (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 43), 9, 1, exit_code);
    scene->top_speed_text   = create_text(speed_text,    (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 53), 9, 1, exit_code);
    scene->handling_text    = create_text(handling_text, (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 63), 9, 1, exit_code);
    
    scene->info_line1 = create_text(car->info_text[0], (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(10, 150), 9, 1, exit_code);
    scene->info_line2 = create_text(car->info_text[1], (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(10, 160), 9, 1, exit_code);
    
    *exit_code = EXIT_SUCCESS;
    return;
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
    render_button(&target->prev_button);
    render_button(&target->next_button);

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