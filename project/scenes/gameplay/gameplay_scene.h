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
#include "../car.h"                                        /// `*car_ptr` (i.e. player).
#include "pause_screen.h"                                  /// Pause screen.
#include "traffic_manager.h"                               /// Traffic manager.
#include "../../game_components/environment.h"             /// `struct Environment`.
#include "../../game_components/multi_texture.h"           /// Multi-textures.
#include "../../game_components/shifting_texture.h"        /// Shifting textures.
#include "../../game_components/texture.h"                 /// Textures.
#include "../../game_components/movement/move_component.h" /// `struct Move_Component`.
#include "../../graphics/graphics_layer.h"              /// `graphics_layer.renderer`.

#define GAMEPLAY_DATA_LINES 15


/* Struct */

struct Gameplay_Scene
{
    SDL_Texture* sky_bg; /// Not a `Texture`, because it's rendered on the whole screen.
    struct Shifting_Texture ground;
    struct Shifting_Texture road;
    struct Shifting_Texture stripes;
    struct Environment      trees;

    struct Car* car_ptr;
    struct Pause_Screen pause_screen;
};


/* Predef */

struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code);
void                  free_gameplay_scene(struct Gameplay_Scene* target);
void                  render_gameplay_scene(struct Gameplay_Scene* target);


/* Body */

struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code)
{
    struct Gameplay_Scene result;
    result.sky_bg  = NULL;
    result.car_ptr = NULL;

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
    struct Deinit_Stack deinit_stack = new_deinit_stack(6, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
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
    result.ground = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 4, 150, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the ground texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.ground, (void (*)(void*))free_shifting_texture); ///  TODO: try replacing it with an incorrect method and see what happens?
    add_to_shifting_texture(&result.ground, scene_data[1], exit_code); /// TODO: find a way
    add_to_shifting_texture(&result.ground, scene_data[2], exit_code); /// to check `exit_code`
    add_to_shifting_texture(&result.ground, scene_data[3], exit_code); /// w/o obnoxious
    add_to_shifting_texture(&result.ground, scene_data[4], exit_code); /// `if() {}` blocks.

    /// Road
    result.road = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 3, 100, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the road texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    add_to_shifting_texture(&result.road, scene_data[5], exit_code);
    add_to_shifting_texture(&result.road, scene_data[6], exit_code);
    add_to_shifting_texture(&result.road, scene_data[7], exit_code);
    add_to_deinit_stack(&deinit_stack, &result.road, (void (*)(void*))free_shifting_texture);
    
    /// Stripes
    result.stripes = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 3, 100, exit_code); /// TEMP: Was 4.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the stripes texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    add_to_shifting_texture(&result.stripes, scene_data[8], exit_code);
    add_to_shifting_texture(&result.stripes, scene_data[9], exit_code);
    add_to_shifting_texture(&result.stripes, scene_data[10], exit_code);
    add_to_deinit_stack(&deinit_stack, &result.stripes, (void (*)(void*))free_shifting_texture);

    /// Trees
    result.trees = new_environment((char*[]){scene_data[11], scene_data[12], scene_data[13], scene_data[14]}, 4, 7, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the trees", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    /// - Tree movement
    struct Path tree_path = new_path(
        (SDL_FRect[]){{80,75, 7, 7},  {75,70,10,10},  {65,65,15,15},
                      {55,60,22,22},  {35,60,30,30},  /*{10,60,40,40},*/
                       {5,55,45,45}, {-30,50,60,60}, {-60,45,75,75}},
                       8, exit_code /// TODO: exit code check.
    );
    struct Move_Component* tree_move_component = malloc(sizeof(struct Move_Component));
    *tree_move_component = init_move_component(tree_path, 150, true, exit_code); /// TODO: exit code check.
    couple_move_component_to_environment(&result.trees, tree_move_component, vec2(10,0), exit_code); /// TODO: exit code check.
    add_to_deinit_stack(&deinit_stack, &result.trees, (void (*)(void*))free_environment); /// Don't delete. More elements will be added later

    result.pause_screen = init_pause_screen(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't create pause screen", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.pause_screen, (void (*)(void*))free_pause_screen);

    init_traffic_manager(5, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't initialize traffic manager", NON_SDL_ERROR);
        free_traffic_manager();
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }

    free_deinit_stack(&deinit_stack); /// `free` because those resources will be used.
    free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_gameplay_scene(struct Gameplay_Scene* target)
{
    if (target == NULL)
        return;
    
    free_pause_screen(&target->pause_screen);
    target->car_ptr->coords.x = center_x(target->car_ptr->coords.w);
    free_environment     (&target->trees);
    free_shifting_texture(&target->stripes);
    free_shifting_texture(&target->road);
    free_shifting_texture(&target->ground);
    if (target->sky_bg != NULL && target->sky_bg != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->sky_bg);
        target->sky_bg = NULL;
    }
}


void render_gameplay_scene(struct Gameplay_Scene* target)
{
    /// Param checking
    if (target == NULL || target->sky_bg == NULL) /// TODO: check all members.
    {
        print_error("`render_gameplay_scene()`: `target` arg or one of its members is `NULL`", NON_SDL_ERROR);
        return;
    }

    if (target->pause_screen.is_open)
    {
        render_pause_screen(&target->pause_screen);
        return;
    }
    
    move_traffic();

    /// Rendering
    SDL_RenderTexture(graphics_layer.renderer, target->sky_bg, NULL, NULL);
    render_traffic_on_pts(0, 0);
    partly_render_environment(&target->trees, 0, 2);
    render_shifting_texture(&target->ground);
    render_shifting_texture(&target->road);
    render_shifting_texture(&target->stripes);
    render_traffic_on_pts(1, 9);
    partly_render_environment(&target->trees, 3, ULONG_LONG_MAX);
    render_car(target->car_ptr);
    render_traffic_on_pts(10, UINT_MAX);
    return;
}

#endif /// GAMEPLAY_SCENE_H