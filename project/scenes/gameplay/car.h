#pragma once
#ifndef CAR_H
#define CAR_H

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* Helper headers */
#include "../../debug.h"           /// Error printing.
#include "../../deinit_stack.h"    /// Deinitialization stack.
#include "../../resources.h"       /// File reading & null texture.
#include "../../helpers/helpers.h" /// `free_ptr_arr()`.

#define CAR_DATA_LINES 7

/// NOTE: a split into `Player_Car` and `Traffic_Car` may happen later.


/* Struct */

struct Car
{
    SDL_Texture* textures[5];
    size_t base_texture;
    SDL_FRect coords;
    int handling;
    int top_speed;
    int direction_x;
    ///unsigned int latest_jump_tick;
};
struct Car load_car(const char path[], int* exit_code);
void       free_car(struct Car* target);


/* Body */

struct Car load_car(const char path[], int* exit_code)
{
    struct Car result;
    result.handling =  0;
    result.top_speed = 0;
    result.direction_x  = 0;
    result.base_texture = 2; /// main texture
    ///result.latest_jump_tick = SDL_GetTicks();

    /// Checking params
    if (exit_code == NULL)
        print_warning("`load_car()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (path == NULL)
    {
        print_error("`load_car()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.coords = (SDL_FRect){center_x(50.0), RENDER_HEIGHT-50.0, 50.0, 50.0}; /// TODO: size determined by real-life size.

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
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)car_data, CAR_DATA_LINES);
        return result;
    }

    /// Textures
    size_t i = 0;
    for ( ; i < 5; i++)
    {
        result.textures[i] = IMG_LoadTexture(graphics_layer.renderer, car_data[i]);
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

    /// Data
    result.handling  = atoi(car_data[i++]);
    result.top_speed = atoi(car_data[i++]);
    
    free_ptr_arr((void**)car_data, CAR_DATA_LINES);
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
    free(target->textures);
    target->handling  = 0;
    target->top_speed = 0;
    target->direction_x = 0;
    return;
}

#endif /// CAR_H