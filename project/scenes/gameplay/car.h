#pragma once
#ifndef CAR_H
#define CAR_H

#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL_image.
#include <stdio.h> /// `getline()`.
#include "../../debug.h" /// Error printing.
#include "../../graphics/texture.h" /// Car texture.


/* Struct */

struct Car
{
    SDL_Texture* texture_main;
    SDL_Texture* texture_right;
    SDL_Texture* texture_left;
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

    /* Reading data */
    FILE* data_file = fopen(path, "r");
    if (data_file == NULL)
    {
        print_error("`load_car()`: could not open data file",  NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// REDO: This approach is very static. Use JSON.
    char* data_line = malloc(64*sizeof(char));
    if (data_line == NULL)
    {
        print_error("`load_car()`: could not allocate memory for data line", NON_SDL_ERROR);
        fclose(data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.coords = (SDL_FRect){center_x(50.0), RENDER_HEIGHT-50.0, 50.0, 50.0}; /// TODO: size determined by real-life size.

    /* Main texture */
    if (fgets(data_line, 64, data_file) == NULL)
    {
        print_error("`load_car()`: premature EOF (line 1, main texture)", NON_SDL_ERROR);
        fclose(data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    else
    {
        data_line[strcspn(data_line, "\n")] = 0; /// `\n` sanitization
        result.texture_main = IMG_LoadTexture(graphics_layer.renderer, data_line);
        if (result.texture_main == NULL)
        {
            print_error("`load_car()`: couldn't load main texture", IS_SDL_ERROR);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }

    /* Left texture */
    if (fgets(data_line, 64, data_file) == NULL)
    {
        print_error("`load_car()`: premature EOF (line 2, left texture)", NON_SDL_ERROR);
        fclose(data_file);
        SDL_DestroyTexture(result.texture_main);
        result.texture_main = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }
    else
    {
        data_line[strcspn(data_line, "\n")] = 0; /// `\n` sanitization
        result.texture_left = IMG_LoadTexture(graphics_layer.renderer, data_line);
        if (result.texture_left == NULL)
        {
            print_error("`load_car()`: couldn't load left texture", IS_SDL_ERROR);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }

    /* Right texture */
    if (fgets(data_line, 64, data_file) == NULL)
    {
        print_error("`load_car()`: premature EOF (line 3, right texture)", NON_SDL_ERROR);
        fclose(data_file);
        SDL_DestroyTexture(result.texture_main);
        result.texture_main = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }
    else
    {
        data_line[strcspn(data_line, "\n")] = 0; /// `\n` sanitization
        result.texture_right = IMG_LoadTexture(graphics_layer.renderer, data_line);
        if (result.texture_right == NULL)
        {
            print_error("`load_car()`: couldn't load right texture", IS_SDL_ERROR);
            SDL_DestroyTexture(result.texture_left);
            result.texture_left = NULL;
            SDL_DestroyTexture(result.texture_main);
            result.texture_main = NULL;
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }

    /* Handling */
    if (fgets(data_line, 64, data_file) == NULL)
    {
        print_error("`load_car()`: premature EOF (line 4, handling)", NON_SDL_ERROR);
        fclose(data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    else
    {
        data_line[strcspn(data_line, "\n")] = 0; /// `\n` sanitization
        result.handling = atof(data_line);
    }
    
    /* Top speed */
    if (fgets(data_line, 64, data_file) == NULL)
    {
        print_error("`load_car()`: premature EOF (line 5, top_speed)", NON_SDL_ERROR);
        fclose(data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    else
    {
        data_line[strcspn(data_line, "\n")] = 0; /// `\n` sanitization
        result.top_speed = atof(data_line);
    }

    fclose(data_file);
    data_file = NULL;
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_car(struct Car* target)
{
    if (target == NULL)
        return;
    
    /// TODO: check for `NULL`.
    SDL_DestroyTexture(target->texture_main);
    target->texture_main = NULL;
    SDL_DestroyTexture(target->texture_left);
    target->texture_left = NULL;
    SDL_DestroyTexture(target->texture_right);
    target->texture_right = NULL;
    target->handling  = 0.0;
    target->top_speed = 0.0;
    target->direction_x = 0;
    return;
}

#endif /// CAR_H