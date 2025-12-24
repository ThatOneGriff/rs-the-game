#pragma once
#ifndef GAMEPLAY_SCENE_H
#define GAMEPLAY_SCENE_H

#include <SDL3/SDL.h>             /// SDL3
#include <SDL3_image/SDL_image.h> /// SDL3_image

#include "../../debug.h" /// Error printing.
#include "gameplay_controls.h" /// Controls.
#include "car.h"               /// car_ptr (player).

#include "../../graphics/multi_texture.h" /// Textures
#include "../../graphics/shifting_texture.h"
#include "../../graphics/texture.h"       /// Multi-textures

#define RSGSDT_LINES 3 /// R.S. Gameplay target Data.


/* Struct */

struct Gameplay_Scene
{
    SDL_Texture* sky_bg; /// Not a `Texture`, because it's rendered on the whole screen.
    struct Texture ground;
    struct Multi_Texture trees;

    struct Car* car_ptr;
};
struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code);
void                  free_gameplay_scene(struct Gameplay_Scene* target);
void                render_gameplay_scene(struct Gameplay_Scene* target, int* exit_code);


/* Body */

struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code)
{
    struct Gameplay_Scene result;
    result.sky_bg = NULL;

    /* Param checking*/
    if (exit_code == NULL)
        print_warning("`load_gameplay_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (path == NULL)
    {
        print_error("`load_gameplay_scene()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (car_ptr == NULL)
    {
        print_error("`load_gameplay_scene()`: `car_ptr` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    else
        result.car_ptr = car_ptr;

    /* Reading the file */
    char** scene_data = read_file_by_line(path, RSGSDT_LINES);
    if (scene_data == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't read data from file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /* Resource loading*/
    result.sky_bg = IMG_LoadTexture(graphics_layer.renderer, scene_data[0]);
    if (result.sky_bg == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't load the sky texture", IS_SDL_ERROR);
        for (size_t i = 0; i < RSGSDT_LINES; i++)
        {
            free(scene_data[i]);
            scene_data[i] = NULL;
        }
        free(scene_data);
        scene_data = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.ground = load_texture(scene_data[1], (SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, exit_code); /// TODO: h=80 && better picture
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the ground texture", NON_SDL_ERROR);
        SDL_DestroyTexture(result.sky_bg);
        result.sky_bg = NULL;
        for (size_t i = 0; i < RSGSDT_LINES; i++)
        {
            free(scene_data[i]);
            scene_data[i] = NULL;
        }
        free(scene_data);
        scene_data = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.trees = load_multi_texture(scene_data[2], 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the trees", NON_SDL_ERROR);
        free_texture(&result.ground);
        SDL_DestroyTexture(result.sky_bg);
        result.sky_bg = NULL;
        for (size_t i = 0; i < RSGSDT_LINES; i++)
        {
            free(scene_data[i]);
            scene_data[i] = NULL;
        }
        free(scene_data);
        scene_data = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    add_to_multi_texture(&result.trees, (SDL_FRect){0, 0, 50, 50}, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't add coordinates to the trees", NON_SDL_ERROR);
        free_multi_texture(&result.trees);
        free_texture(&result.ground);
        SDL_DestroyTexture(result.sky_bg);
        result.sky_bg = NULL;
        for (size_t i = 0; i < RSGSDT_LINES; i++)
        {
            free(scene_data[i]);
            scene_data[i] = NULL;
        }
        free(scene_data);
        scene_data = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    for (size_t i = 0; i < RSGSDT_LINES; i++)
    {
        free(scene_data[i]);
        scene_data[i] = NULL;
    }
    free(scene_data);
    scene_data = NULL;
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_gameplay_scene(struct Gameplay_Scene* target)
{
    if (target == NULL)
        return;
    if (target->sky_bg != NULL)
    {
        SDL_DestroyTexture(target->sky_bg);
        target->sky_bg = NULL;
    }
    target->car_ptr->coords.x = center_x(target->car_ptr->coords.w);
    free_texture(&target->ground);
    free_multi_texture(&target->trees);
}


/// Renders into `graphics_layer`'s `buffer` texture.
void render_gameplay_scene(struct Gameplay_Scene* target, int* exit_code)
{
    /* Param checking*/
    if (exit_code == NULL)
        print_warning("`render_gameplay_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (target == NULL || target->sky_bg == NULL) /// TODO: take all members into account.
    {
        print_error("`render_gameplay_scene()`: `target` arg or one of its members is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    process_gameplay_car_input(target->car_ptr);

    /* Rendering */
    SDL_RenderTexture(graphics_layer.renderer, target->sky_bg, NULL, NULL);
    render_texture(&target->ground);
    render_multi_texture(&target->trees);
    SDL_RenderTexture(graphics_layer.renderer, target->car_ptr->textures[target->car_ptr->base_texture], NULL, &target->car_ptr->coords);
    *exit_code = EXIT_SUCCESS;
    return;
}

#endif /// GAMEPLAY_SCENE_H