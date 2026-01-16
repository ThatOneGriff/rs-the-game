#pragma once
#ifndef CAR_H
#define CAR_H

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* Helper headers */
#include "../debug.h"             /// Error printing.
#include "../deinit_stack.h"      /// Deinitialization stack.
#include "../resources.h"         /// File reading & null texture.
#include "../helpers/helpers.h"   /// `free_ptr_arr()`.
#include "../logic/logic_layer.h" /// `time_tick_ms`

#define         CAR_DATA_LINES 16
#define TRAFFIC_CAR_DATA_LINES  4


/* Struct */

struct Car
{
    char name[25];
    int  year;
    int  hp;
    int  top_speed;
    int  handling;
    char quad_paths[4][64];
    char info_text [2][50];

    SDL_Texture* textures[5];
    size_t       base_texture;

    SDL_FRect     coords;
    int           direction_x;
    int prev_turn_direction_x; /// Saves the latest non-default `direction_x`. Needed for turn smoothing.

    time_tick_ms latest_turn_start;
    time_tick_ms latest_turn_end;
    time_tick_ms turn_smoothing_duration;
    ///time_tick_ms latest_jump_tick;
};


/* Predef */

struct Car load_car        (const char path[], int* exit_code);
struct Car load_traffic_car(const char path[], int* exit_code);
void       free_car        (struct Car* target);
void     render_car        (struct Car* target);


/* Body */

struct Car load_car(const char path[], int* exit_code)
{
    struct Car result;
    result.year = 0;
    result.hp   = 0;
    result.top_speed = 0;
    result.handling  = 0;

    result.base_texture = 0;
    result.direction_x           = 0;
    result.prev_turn_direction_x = 0;
    result.base_texture = 2; /// main texture
    result.latest_turn_start = 0; /// Temporary value to be reset with the first turn.
    result.latest_turn_end   = 0; /// Temporary value to be reset with the first turn.
    result.turn_smoothing_duration = 100;
    ///result.latest_jump_tick = 0;

    /// Checking params
    if (exit_code == NULL)
        print_warning("`load_car()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (path == NULL)
    {
        print_error("`load_car()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.coords = (SDL_FRect){center_x(65.0), RENDER_HEIGHT-65.0, 65.0, 65.0}; /// IDEA: size determined by real-life size.

    /// Reading data
    char** car_data = read_file_by_line(path, CAR_DATA_LINES);
    if (car_data == NULL)
    {
        print_error("`load_car()`: couldn't read the contents of car data file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(5, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_car()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)car_data, CAR_DATA_LINES);
        return result;
    }

    strcpy(result.name, car_data[0]);
    result.year = atoi(car_data[1]);
    result.hp   = atoi(car_data[2]);
    result.top_speed = atoi(car_data[3]);
    result.handling  = atoi(car_data[4]);
    strcpy(result.quad_paths[0], car_data[5]);
    strcpy(result.quad_paths[1], car_data[6]);
    strcpy(result.quad_paths[2], car_data[7]);
    strcpy(result.quad_paths[3], car_data[8]);
    strcpy(result.info_text [0], car_data[9]);
    strcpy(result.info_text [1], car_data[10]);

    /// Textures (gameplay)
    for (size_t i = 0; i < 5; i++)
    {
        result.textures[i] = IMG_LoadTexture(graphics_layer.renderer, car_data[i+11]);
        if (result.textures[i] == NULL)
        {
            if (NULL_TEXTURE != NULL)
            {
                print_warning("`load_car()`: couldn't load texture, replaced with null texture", IS_SDL_ERROR);
                result.textures[i] = NULL_TEXTURE;
            }
            else
            {
                print_error("`load_car()`: couldn't load texture, and null texture is empty", IS_SDL_ERROR);
                flush_deinit_stack(&deinit_stack);
                free_ptr_arr((void**)car_data, CAR_DATA_LINES);
                *exit_code = EXIT_FAILURE;
                return result;
            }
        }
        if (result.textures[i] != NULL_TEXTURE)
            add_to_deinit_stack(&deinit_stack, result.textures[i], (void (*)(void*))SDL_DestroyTexture);
    }
    
    free_ptr_arr((void**)car_data, CAR_DATA_LINES);
    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return result;
}


struct Car load_traffic_car(const char path[], int* exit_code)
{
    /// Filling unneeded variables with zeroes
    struct Car result;
    result.year = 0;
    result.hp   = 0;
    result.top_speed = 0; /// NOTE: may be useful later.
    result.handling  = 0;
    for (size_t i = 0; i < 4; i++)
        strcpy(result.quad_paths[i], "\0");
    for (size_t i = 0; i < 2; i++)
        strcpy(result.info_text [i], "\0");
    result.base_texture = 0;
    result.direction_x           = 0;
    result.prev_turn_direction_x = 0;
    result.base_texture = 2; /// main texture
    result.latest_turn_start = 0;
    result.latest_turn_end   = 0;
    result.turn_smoothing_duration = 0;

    /// Checking params
    if (exit_code == NULL)
        print_warning("`load_traffic_car()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (path == NULL)
    {
        print_error("`load_traffic_car()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Reading data
    char** car_data = read_file_by_line(path, TRAFFIC_CAR_DATA_LINES);
    if (car_data == NULL)
    {
        print_error("`load_traffic_car()`: couldn't read the contents of car data file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(3, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_traffic_car()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)car_data, TRAFFIC_CAR_DATA_LINES);
        return result;
    }

    strcpy(result.name, car_data[0]);

    size_t data_i = 1;
    /// Textures (gameplay)
    for (size_t i = 0; i < 5; i++)
    {
        if (i % 2 == 1) /// i.e. for textures[1,3], which are the 'intermediate' ones and not needed.
        {
            result.textures[i] = NULL_TEXTURE;
            continue;
        }

        result.textures[i] = IMG_LoadTexture(graphics_layer.renderer, car_data[data_i++]);
        if (result.textures[i] == NULL)
        {
            if (NULL_TEXTURE != NULL)
            {
                print_warning("`load_traffic_car()`: couldn't load texture, replaced with null texture", IS_SDL_ERROR);
                result.textures[i] = NULL_TEXTURE;
            }
            else
            {
                print_error("`load_traffic_car()`: couldn't load texture, and null texture is empty", IS_SDL_ERROR);
                flush_deinit_stack(&deinit_stack);
                free_ptr_arr((void**)car_data, TRAFFIC_CAR_DATA_LINES);
                *exit_code = EXIT_FAILURE;
                return result;
            }
        }
        if (result.textures[i] != NULL_TEXTURE)
            add_to_deinit_stack(&deinit_stack, result.textures[i], (void (*)(void*))SDL_DestroyTexture);
    }
    
    free_ptr_arr((void**)car_data, TRAFFIC_CAR_DATA_LINES);
    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_car(struct Car* target)
{
    if (target == NULL)
        return;
    
    for (size_t i = 0; i < 5; i++)
    {
        if (target->textures[i] != NULL_TEXTURE)
        {
            SDL_DestroyTexture(target->textures[i]);
            target->textures[i] = NULL;
        }
    }
    target->handling  = 0;
    target->top_speed = 0;
    target->direction_x = 0;
    return;
}


void render_car(struct Car* target)
{
    if (target == NULL) /// TODO: check for everything.
    {
        print_error("`render_car()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    SDL_RenderTexture(graphics_layer.renderer, target->textures[target->base_texture], NULL, &target->coords);
    return;
}

#endif /// CAR_H