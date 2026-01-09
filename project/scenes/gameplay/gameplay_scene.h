#pragma once
#ifndef GAMEPLAY_SCENE_H
#define GAMEPLAY_SCENE_H

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* Helper headers */
#include "../../debug.h"           /// Error printing.
#include "../../deinit_stack.h"    /// Deinitialization stack.
#include "../../resources.h"       /// Null texture.
#include "../../helpers/helpers.h" /// `free_ptr_arr()`.

/* Graphics & components */
#include "car.h"                                    /// `*car_ptr` (i.e. player).
#include "../../game_components/environment.h"      /// `struct Environment`.
#include "../../game_components/move_component.h"   /// `struct Move_Component`.
#include "../../game_components/multi_texture.h"    /// Multi-textures.
#include "../../game_components/shifting_texture.h" /// Shifting textures.
#include "../../game_components/texture.h"          /// Textures.
#include "../../graphics/graphics_layer.h"          /// `graphics_layer.renderer`.

#define GAMEPLAY_DATA_LINES 12


/* Struct */

struct Gameplay_Scene
{
    SDL_Texture* sky_bg; /// Not a `Texture`, because it's rendered on the whole screen.
    struct Shifting_Texture ground;
    struct Shifting_Texture road;
    struct Environment      trees;

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
            free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
            *exit_code = EXIT_FAILURE;
            return result;
        }
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
    if (result.sky_bg != NULL_TEXTURE)
        add_to_deinit_stack(&deinit_stack, result.sky_bg, (void (*)(void*))SDL_DestroyTexture);

    /// Ground
    result.ground = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 4, 150, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the ground texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.ground, (void (*)(void*))free_shifting_texture); ///  TODO: try replacing it with an incorrect method and see what happens?
    add_to_shifting_texture(&result.ground, scene_data[1], exit_code); /// TODO: find a way
    add_to_shifting_texture(&result.ground, scene_data[2], exit_code); /// to check `exit_code`
    add_to_shifting_texture(&result.ground, scene_data[3], exit_code); /// w/o obnoxious
    add_to_shifting_texture(&result.ground, scene_data[4], exit_code); /// `if() {}` blocks.

    /// Road
    result.road = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 4, 150, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the ground texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    add_to_shifting_texture(&result.road, scene_data[5], exit_code);
    add_to_shifting_texture(&result.road, scene_data[6], exit_code);
    add_to_shifting_texture(&result.road, scene_data[7], exit_code);
    add_to_shifting_texture(&result.road, scene_data[8], exit_code);
    add_to_deinit_stack(&deinit_stack, &result.road, (void (*)(void*))free_shifting_texture);

    /// Trees
    result.trees = new_environment((char*[]){scene_data[9], scene_data[10], scene_data[11]}, 3, 7, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the trees", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    /// - Tree movement
    struct Path tree_path = new_path(
        (SDL_FRect[]){{80,75,10,10},  {70,65,15,15},  {60,60,20,20},
                      {45,55,25,25},  {30,60,30,30},  {15,65,35,35},
                       {0,70,40,40}, {-15,75,45,45}, {-30,80,50,50},
                     {-45,85,55,55}, {-60,90,60,60}}, 11, exit_code /// TODO: exit code check.
    );
    struct Move_Component* tree_move_component = malloc(sizeof(struct Move_Component));
    *tree_move_component = init_move_component(tree_path, 150, true, exit_code); /// TODO: exit code check.
    couple_move_component_to_environment(&result.trees, tree_move_component, vec2(50,0), exit_code); /// TODO: exit code check.
    add_to_deinit_stack(&deinit_stack, &result.trees, (void (*)(void*))free_environment); /// Don't delete. More elements will be added later

    free_deinit_stack(&deinit_stack); /// `free` because those resources will be used.
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
    partly_render_environment(&target->trees, 0, 2);
    render_shifting_texture(&target->ground);
    render_shifting_texture(&target->road);
    partly_render_environment(&target->trees, 3, UINT_MAX);
    SDL_RenderTexture(graphics_layer.renderer, target->car_ptr->textures[target->car_ptr->base_texture], NULL, &target->car_ptr->coords);
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_gameplay_scene(struct Gameplay_Scene* target)
{
    if (target == NULL)
        return;
    
    target->car_ptr->coords.x = center_x(target->car_ptr->coords.w);
    free_environment     (&target->trees);
    free_shifting_texture(&target->road);
    free_shifting_texture(&target->ground);
    if (target->sky_bg != NULL && target->sky_bg != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->sky_bg);
        target->sky_bg = NULL;
    }
}

#endif /// GAMEPLAY_SCENE_H