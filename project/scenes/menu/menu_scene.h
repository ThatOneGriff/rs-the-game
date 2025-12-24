#pragma once
#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include <SDL3/SDL.h> /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

#include <stdlib.h> /// `malloc()`

#include "menu_controls.h" /// Controls.
#include "../../debug.h"   /// Error printing.
#include "../../helpers/helpers.h" /// Colors.
#include "../../graphics/graphics_layer.h" /// `graphics_layer`.
#include "../../text/border.h" /// Selected text highlighting.
#include "../../text/text.h"   /// Text.

#define RSMSDT_LINES 1 /// R.S. Menu Scene Data.


/* Struct */

struct Menu_Scene
{
    SDL_Texture* bg;
    
    struct Texture car_name_text; /// TEMP, or to be moved into menu car management.
    /*struct Texture main_car_picture;
    struct Texture extra_car_pictures[4];

    struct Texture FPS_text;
    struct Texture FPS_value_text;

    struct Texture play_text;

    void* selectable;
    size_t selected;*/
};
struct Menu_Scene load_menu_scene(const char path[], int* exit_code);
void              free_menu_scene(struct Menu_Scene* target);
void            render_menu_scene(struct Menu_Scene* target, int* exit_code);


/* Functions */

struct Menu_Scene load_menu_scene(const char path[], int* exit_code)
{
    struct Menu_Scene result;
    result.bg = NULL;

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

    result.car_name_text = create_text("Renault Clio Williams", SDL_WHITE, SDL_WHITE, 200, 0, exit_code);
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
    //result.main_car_picture = load_texture("")
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_menu_scene(struct Menu_Scene* target)
{
    SDL_DestroyTexture(target->bg);
    free_texture(&target->car_name_text);
}


void render_menu_scene(struct Menu_Scene* target, int* exit_code)
{
    SDL_RenderTexture(graphics_layer.renderer, target->bg, NULL, NULL);
    render_texture(&target->car_name_text);
    *exit_code = EXIT_SUCCESS;
    return;
}

#endif /// MENU_SCENE_H