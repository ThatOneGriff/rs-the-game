#pragma once
#ifndef CAR_H
#define CAR_H

#include <stdio.h> /// `getline()`
#include "../../debug.h" /// Error printing
#include "../../graphics/texture.h" /// Car texture


/* Struct */

struct Car
{
    struct Texture texture;
    float handling;
    float top_speed;
};
struct Car load_car(const char path[], int* exit_code);
void       free_car(struct Car* target);


/* Body */

struct Car load_car(const char path[], int* exit_code)
{
    struct Car result;
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

    /* Texture */
    if (fgets(data_line, 64, data_file) == NULL)
    {
        print_error("`load_car()`: premature EOF (line 1, texture)", NON_SDL_ERROR);
        fclose(data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    else
    {
        data_line[strcspn(data_line, "\n")] = 0; /// `\n` sanitization
        result.texture = load_texture(data_line, (SDL_FRect){center_x(50.0), RENDER_HEIGHT-50.0, 50.0, 50.0}, exit_code); /// TEMP sizes
        if (*exit_code == EXIT_FAILURE)
            return result;
    }

    /* Handling */
    if (fgets(data_line, 64, data_file) == NULL)
    {
        print_error("`load_car()`: premature EOF (line 2, handling)", NON_SDL_ERROR);
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
        print_error("`load_car()`: premature EOF (line 3, top_speed)", NON_SDL_ERROR);
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
    
    free_texture(&target->texture);
    target->handling  = 0.0;
    target->top_speed = 0.0;
    return;
}

#endif /// CAR_H