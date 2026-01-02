#pragma once
#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include <SDL3/SDL.h> /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

#include <stdlib.h> /// `malloc()`

#include "../../debug.h"   /// Error printing.
#include "../../helpers/helpers.h" /// Colors.
#include "../../graphics/graphics_layer.h" /// `graphics_layer`.
#include "../../graphics/button.h"         /// Button
#include "../../graphics/texture.h"        /// Texture
#include "../../text/border.h" /// Selected text highlighting.
#include "../../text/text.h"   /// Text.

#define RSMSDT_LINES 1 /// R.S. Menu Scene Data.


/* Struct */

struct Menu_Scene
{
    SDL_Texture* bg;
    
    struct Texture car_name_text; /// TEMP, or to be moved into menu car management.
    struct Button play_button;
    struct Button dummy_button;
    //struct Button* buttons[2];
    //size_t button_focus_i;
    /*struct Texture main_car_picture;
    struct Texture extra_car_pictures[4];

    struct Texture FPS_text;
    struct Texture FPS_value_text;

    void* selectable;
    size_t selected;*/
};
struct Menu_Scene load_menu_scene(const char path[], int* exit_code);
void              menu_scene_tick(struct Menu_Scene* target, int* exit_code);
void              free_menu_scene(struct Menu_Scene* target);


/* Functions */

struct Menu_Scene load_menu_scene(const char path[], int* exit_code)
{
    struct Menu_Scene result;
    result.bg = NULL;
    //result.button_focus_i = 0;

    /* Param checking*/
    if (exit_code == NULL)
        print_warning("`load_menu_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (path == NULL)
    {
        print_error("`load_menu_scene()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    
    /* Reading the file */
    char** scene_data = read_file_by_line(path, RSMSDT_LINES);
    if (scene_data == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't read data from file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.bg = IMG_LoadTexture(graphics_layer.renderer, scene_data[0]);
    if (result.bg == NULL)
    {
        print_error("`load_menu_scene()`: couldn't load the bg texture", IS_SDL_ERROR);
        for (size_t i = 0; i < RSMSDT_LINES; i++)
        {
            free(scene_data[i]);
            scene_data[i] = NULL;
        }
        free(scene_data);
        scene_data = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.car_name_text = create_text("Renault Clio Williams", SDL_WHITE, SDL_WHITE, vec2(100, 50), 15, 0, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create the text", IS_SDL_ERROR);
        SDL_DestroyTexture(result.bg);
        result.bg = NULL;
        for (size_t i = 0; i < RSMSDT_LINES; i++)
        {
            free(scene_data[i]);
            scene_data[i] = NULL;
        }
        free(scene_data);
        scene_data = NULL;
        *exit_code = EXIT_FAILURE;
    }
    
    result.dummy_button = create_button("dummy", rand_color(), vec2(20, 20), 10, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create dummy button", IS_SDL_ERROR);
        free_texture(&result.car_name_text);
        SDL_DestroyTexture(result.bg);
        result.bg = NULL;
        for (size_t i = 0; i < RSMSDT_LINES; i++)
        {
            free(scene_data[i]);
            scene_data[i] = NULL;
        }
        free(scene_data);
        scene_data = NULL;
        *exit_code = EXIT_FAILURE;
    }
    result.dummy_button.is_focused = true; /// TEMP??
    //result.buttons[0] = &result.dummy_button;

    result.play_button = create_button("PLAY", rand_color(), vec2(150, 150), 20, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create play button", IS_SDL_ERROR);
        free_button(&result.dummy_button);
        free_texture(&result.car_name_text);
        SDL_DestroyTexture(result.bg);
        result.bg = NULL;
        for (size_t i = 0; i < RSMSDT_LINES; i++)
        {
            free(scene_data[i]);
            scene_data[i] = NULL;
        }
        free(scene_data);
        scene_data = NULL;
        *exit_code = EXIT_FAILURE;
    }
    //result.buttons[1] = &result.play_button;

    //result.main_car_picture = load_texture("")
    *exit_code = EXIT_SUCCESS;
    return result;
}


void menu_scene_tick(struct Menu_Scene* target, int* exit_code)
{
    SDL_RenderTexture(graphics_layer.renderer, target->bg, NULL, NULL);
    render_texture(&target->car_name_text);
    render_button(&target->play_button);
    render_button(&target->dummy_button);

    *exit_code = EXIT_SUCCESS;
    return;
}


void free_menu_scene(struct Menu_Scene* target)
{
    SDL_DestroyTexture(target->bg);
    free_texture(&target->car_name_text);
    free_button(&target->dummy_button);
    free_button(&target->play_button);
}

#endif /// MENU_SCENE_H