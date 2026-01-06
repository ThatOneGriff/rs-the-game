#pragma once
#ifndef GAMEPLAY_SCENE_H
#define GAMEPLAY_SCENE_H

#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

#include "../../deinit_stack.h"    /// Deinitialization stack.
#include "../../helpers/helpers.h" /// `free_ptr_arr()`.
#include "../../resources.h" /// Null texture.
#include "../../debug.h"     /// Error printing.
#include "car.h"             /// `*car_ptr` (player).

#include "../../game_components/multi_texture.h"    /// Textures.
#include "../../game_components/shifting_texture.h" /// Shifting textures.
#include "../../game_components/texture.h"          /// Multi-textures.

#define GAMEPLAY_DATA_LINES 3


/* Struct */

struct Gameplay_Scene
{
    SDL_Texture* sky_bg; /// Not a `Texture`, because it's rendered on the whole screen.
    struct Texture ground;
    struct Multi_Texture trees;

    struct Car* car_ptr;
};
struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code);
void                  gameplay_scene_tick(struct Gameplay_Scene* target, int* exit_code);
void                  free_gameplay_scene(struct Gameplay_Scene* target);


/* Body */

struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code)
{
    struct Gameplay_Scene result;
    result.sky_bg = NULL;

    /// Param checking
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

    /// Reading the file
    char** scene_data = read_file_by_line(path, GAMEPLAY_DATA_LINES);
    if (scene_data == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't read data from file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(4, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }

    /// Sky
    result.sky_bg = IMG_LoadTexture(graphics_layer.renderer, scene_data[0]);
    if (result.sky_bg == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`load_gameplay_scene()`: couldn't load the sky texture, replaced with null texture", IS_SDL_ERROR);
            result.sky_bg = NULL_TEXTURE;
        }
        else
        {
            print_error("`load_gameplay_scene()`: couldn't load the sky texture, and null texture is empty", IS_SDL_ERROR);
            free_deinit_stack(&deinit_stack);
            free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }
    if (result.sky_bg != NULL_TEXTURE)
        add_to_deinit_stack(&deinit_stack, result.sky_bg, (void (*)(void*))SDL_DestroyTexture);

    /// Ground
    result.ground = load_texture(scene_data[1], (SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, exit_code); /// TODO: h=80 && better picture
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the ground texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.ground, (void (*)(void*))free_texture);

    /// Trees
    result.trees = load_multi_texture(scene_data[2], 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the trees", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    add_to_multi_texture(&result.trees, (SDL_FRect){0, 0, 50, 50}, exit_code); /// Will be more trees later
    /// There must be some more elegant way to do this, than simply checking `*exit_code` after each tree's coordinates.
    /// A cycle, maybe?
    /*if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't add coordinates to the trees", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return result;
    }*/
    add_to_deinit_stack(&deinit_stack, &result.trees, (void (*)(void*))free_multi_texture); /// Don't delete. More elements will be added later

    free_deinit_stack(&deinit_stack);
    free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
    *exit_code = EXIT_SUCCESS;
    return result;
}


/// Renders into `graphics_layer`'s `buffer` texture.
void gameplay_scene_tick(struct Gameplay_Scene* target, int* exit_code)
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

    /* Rendering */
    SDL_RenderTexture(graphics_layer.renderer, target->sky_bg, NULL, NULL);
    render_texture(&target->ground);
    render_multi_texture(&target->trees);
    SDL_RenderTexture(graphics_layer.renderer, target->car_ptr->textures[target->car_ptr->base_texture], NULL, &target->car_ptr->coords);
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_gameplay_scene(struct Gameplay_Scene* target)
{
    if (target == NULL)
        return;
    
    if (target->sky_bg != NULL && target->sky_bg != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->sky_bg);
        target->sky_bg = NULL;
    }
    target->car_ptr->coords.x = center_x(target->car_ptr->coords.w);
    free_texture(&target->ground);
    free_multi_texture(&target->trees);
}

#endif /// GAMEPLAY_SCENE_H