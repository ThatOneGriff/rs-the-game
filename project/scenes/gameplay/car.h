#pragma once
#ifndef CAR_H
#define CAR_H

#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL_image.
#include <stdio.h>       /// `getline()`.
#include "../../debug.h"     /// Error printing.
#include "../../resources.h" /// File reading.
#include "../../graphics/texture.h" /// Car texture.


/* Struct */

struct Car
{
    SDL_Texture* textures[5];
    size_t base_texture;
    SDL_FRect coords;
    float handling;
    float top_speed;
    float direction_x;
};
struct Car load_car(const char path[], int* exit_code);
void       free_car(struct Car* target);


/* Body */

struct Car load_car(const char path[], int* exit_code)
{
    struct Car result;
    result.handling = 0.0;
    result.top_speed = 0.0;
    result.direction_x = 0.0;
    result.base_texture = 2; /// main texture

    /* Checking params */
    if (exit_code == NULL)
        print_warning("`load_car()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    else
        *exit_code = EXIT_SUCCESS;
    if (path == NULL)
    {
        print_error("`load_car()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.coords = (SDL_FRect){center_x(50.0), RENDER_HEIGHT-50.0, 50.0, 50.0}; /// TODO: size determined by real-life size.

    /* Reading data */
    char** car_data = read_file_by_line("res/car_data/clio-williams.rscdt", RSCDT_LINES);
    if (car_data == NULL)
    {
        print_error("`load_car()`: couldn't read the contents of car data file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /* Textures */
    size_t i = 0;
    for ( ; i < 5; i++)
    {
        result.textures[i] = IMG_LoadTexture(graphics_layer.renderer, car_data[i]);
        if (result.textures[i] == NULL)
        {
            print_error("`load_car()`: couldn't load texture", IS_SDL_ERROR);
            for (size_t j = 0; j < i; i++)
            {
                SDL_DestroyTexture(result.textures[j]);
                result.textures[j] = NULL;
            }
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }

    /* Data */
    result.handling  = atoi(car_data[i++]);
    result.top_speed = atoi(car_data[i++]);
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_car(struct Car* target)
{
    if (target == NULL)
        return;
    
    /// TODO: check for `NULL`.
    for (size_t i = 0; i < 5; i++)
    {
        SDL_DestroyTexture(target->textures[i]);
        target->textures[i] = NULL;
    }
    free(target->textures);
    target->handling  = 0.0;
    target->top_speed = 0.0;
    target->direction_x = 0;
    return;
}

#endif /// CAR_H