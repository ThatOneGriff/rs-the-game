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
    struct Texture car_name_text; /// TEMP, or to be moved into a `Car`'s menu display management.
    struct Button play_button;
    struct Button quit_button;

    struct Texture photo_quad1;
    struct Texture photo_quad2;
    struct Texture photo_quad3;
    struct Texture photo_quad4;

    //struct Button* buttons[2];
    //size_t button_focus_i;

    /*struct Texture main_car_picture;
    struct Texture extra_car_pictures[4];

    void* selectable;
    size_t selected;*/
};
struct Menu_Scene load_menu_scene(int* exit_code);
void              menu_scene_tick(struct Menu_Scene* target, int* exit_code);
void              free_menu_scene(struct Menu_Scene* target);


/* Functions */

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
    result.car_name_text = create_text("Renault Clio Williams", (SDL_Color){255,255,255,255}, (SDL_Color){255,255,255,255}, vec2(15, 10), 15, 0, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create the text", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        *exit_code = EXIT_FAILURE;
    }
    add_to_deinit_stack(&deinit_stack, &result.car_name_text, (void (*)(void*))free_texture);

    /// Play button
    result.play_button = create_button("PLAY", (SDL_Color){255,214,90,255}, vec2(15, 150), 25, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create play button", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        *exit_code = EXIT_FAILURE;
    }
    result.play_button.is_focused = true;
    add_to_deinit_stack(&deinit_stack, &result.play_button, (void (*)(void*))free_button);

    /// Quit button
    result.quit_button = create_button("QUIT", (SDL_Color){127,140,170,255}, vec2(100, 150), 25, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create quit button", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        *exit_code = EXIT_FAILURE;
    }
    result.quit_button.is_focused = false;
    add_to_deinit_stack(&deinit_stack, &result.quit_button, (void (*)(void*))free_button);

    /// Photo quads. REDO: very hard-coded and unsafe as of now.
    result.photo_quad1 = load_texture("./res/images/cars/clio-williams/quad1.png", (SDL_FRect){15   , 34   , 72, 54}, exit_code);
    result.photo_quad2 = load_texture("./res/images/cars/clio-williams/quad2.png", (SDL_FRect){15+72, 34   , 72, 54}, exit_code);
    result.photo_quad3 = load_texture("./res/images/cars/clio-williams/quad3.png", (SDL_FRect){15   , 34+54, 72, 54}, exit_code);
    result.photo_quad4 = load_texture("./res/images/cars/clio-williams/quad4.png", (SDL_FRect){15+72, 34+54, 72, 54}, exit_code);

    //result.main_car_picture = load_texture("")
    free_deinit_stack(&deinit_stack); /// `free` because those resources will be used.
    free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void menu_scene_tick(struct Menu_Scene* target, int* exit_code)
{
    SDL_RenderTexture(graphics_layer.renderer, target->bg, NULL, NULL);
    render_texture(&target->car_name_text);
    render_button (&target->play_button);
    render_button (&target->quit_button);
    render_texture(&target->photo_quad1);
    render_texture(&target->photo_quad2);
    render_texture(&target->photo_quad3);
    render_texture(&target->photo_quad4);

    *exit_code = EXIT_SUCCESS;
    return;
}


void free_menu_scene(struct Menu_Scene* target)
{
    if (target->bg != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->bg);
        target->bg = NULL;
    }
    free_texture(&target->car_name_text);
    free_button (&target->play_button);
    free_button (&target->quit_button);
    free_texture(&target->photo_quad1);
    free_texture(&target->photo_quad2);
    free_texture(&target->photo_quad3);
    free_texture(&target->photo_quad4);
}

#endif /// MENU_SCENE_H