#pragma once
#ifndef GAMEPLAY_SCENE_H
#define GAMEPLAY_SCENE_H

#include <SDL3/SDL.h>             /// SDL3
#include <SDL3_image/SDL_image.h> /// SDL3_image

#include "../../debug.h" /// Error printing.
#include "gameplay_controls.h" /// Controls.
#include "car.h"               /// Car (player).

#include "../../graphics/multi_texture.h" /// Textures
#include "../../graphics/shifting_texture.h"
#include "../../graphics/texture.h"       /// Multi-textures


/* Struct */

struct Gameplay_Scene
{
    SDL_Texture* sky_bg; /// Not a `Texture`, because it's rendered on the whole screen.
    struct Texture ground;
    struct Multi_Texture trees;
};
struct Gameplay_Scene load_gameplay_scene(const char* sky_bg_path, const char* ground_path, const char*, int* exit_code);
void                  free_gameplay_scene(struct Gameplay_Scene* target);
void                render_gameplay_scene(struct Gameplay_Scene* scene, struct Car* car, int* exit_code);


/* Body */

struct Gameplay_Scene load_gameplay_scene(const char* sky_bg_path, const char* ground_path, const char* tree_path, int* exit_code)
{
    struct Gameplay_Scene result;

    /* Param checking*/
    if (exit_code == NULL)
        print_warning("`load_gameplay_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (sky_bg_path == NULL || ground_path == NULL || tree_path == NULL)
    {
        print_error("`load_gameplay_scene()`: 1 or more path args is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /* Resource loading*/
    result.sky_bg = IMG_LoadTexture(graphics_layer.renderer, sky_bg_path);
    if (result.sky_bg == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't load the sky texture", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.ground = load_texture(ground_path, (SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, exit_code); /// TODO: h=80 && better picture
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the ground texture", NON_SDL_ERROR);
        return result;
    }

    result.trees = load_multi_texture(tree_path, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the trees", NON_SDL_ERROR);
        return result;
    }

    add_to_multi_texture(&result.trees, (SDL_FRect){0, 0, 50, 50}, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't add coordinates to the trees", NON_SDL_ERROR);
        return result;
    }

    /// TODO: load the rest.

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
    free_texture(&target->ground);
    free_multi_texture(&target->trees);
}


/// Renders into `graphics_layer`'s `buffer` texture.
void render_gameplay_scene(struct Gameplay_Scene* scene, struct Car* car, int* exit_code)
{
    /* Param checking*/
    if (exit_code == NULL)
        print_warning("`render_gameplay_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (scene == NULL || scene->sky_bg == NULL) /// TODO: take all members into account.
    {
        print_error("`render_gameplay_scene()`: `scene` arg or one of its members is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (car == NULL)
    {
        print_error("`render_gameplay_scene()`: `car` is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /* Rendering */
    SDL_RenderTexture(graphics_layer.renderer, scene->sky_bg, NULL, NULL);
    render_texture(&scene->ground);
    render_multi_texture(&scene->trees);

    /* Car rendering */
    car->coords.x += car->direction_x * car->handling * (FPS_manager.delta_ns / SEC_IN_NS);
    if (car->direction_x == 1)
        SDL_RenderTexture(graphics_layer.renderer, car->texture_right, NULL, &car->coords);
    else if (car->direction_x == -1)
        SDL_RenderTexture(graphics_layer.renderer, car->texture_left,  NULL, &car->coords);
    else if (car->direction_x == 0)
        SDL_RenderTexture(graphics_layer.renderer, car->texture_main,  NULL, &car->coords);
    *exit_code = EXIT_SUCCESS;
    return;
}

#endif /// GAMEPLAY_SCENE_H