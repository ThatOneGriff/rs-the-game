#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H

#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.
#include <SDL3_ttf/SDL_ttf.h>     /// SDL3_ttf.
#include "graphics/graphics_layer.h" /// Texture loading (`renderer`).

#include <stdio.h>  /// `strcpy()`
#include <stdlib.h> /// `*alloc()`.
#include "debug.h"  /// Error printing.
#include "helpers/helpers.h" /// `free_ptr_array()`.
#include "deinit_stack.h"    /// Deinitialization.

#define GLOBAL_DATA_PATH "./rsdt/global.rsdt"
#define GLOBAL_DATA_LINES 3

static SDL_Surface* ICON_TEXTURE = NULL;
static SDL_Texture* NULL_TEXTURE = NULL;
static char       MAIN_FONT_PATH[64];

/// NOTE: `.rsdt` is a compromise born from a lack of time. JSON loading is preferable and will be eventually the mechanism.


/* Predef */

void  _load_global_resources(int* exit_code);
void  _free_global_resources(void);
char** read_file_by_line(const char* path, const size_t target_lines);


/* Body */

/// Must be called after SDL, TTF, & `renderer` have been initialized.
/// Called in `init()` or upon resource reloading.
void _load_global_resources(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`load_global_resources()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);

    /// Reading global data file
    char** global_data = read_file_by_line(GLOBAL_DATA_PATH, GLOBAL_DATA_LINES);
    if (global_data == NULL)
    {
        print_error("`load_global_resources()`: couldn't read global data", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(2, exit_code); /// Not adding the last element (font loading). Also, `global_data` needs its own treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_global_resources()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return;
    }

    /// Icon
    ICON_TEXTURE = IMG_Load(global_data[0]);
    if (ICON_TEXTURE == NULL)
    {
        print_error("`load_global_resources()`: couldn't load app icon", IS_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_array((void**)global_data, GLOBAL_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, ICON_TEXTURE, (void (*)(void*))SDL_DestroySurface);

    /// Null texture
    NULL_TEXTURE = IMG_LoadTexture(graphics_layer.renderer, global_data[1]);
    if (NULL_TEXTURE == NULL)
        print_warning("`load_global_resources()`: couldn't load null texture (not critical)", IS_SDL_ERROR);
    else
        add_to_deinit_stack(&deinit_stack, NULL_TEXTURE, (void (*)(void*))SDL_DestroyTexture);

    /// Font (+ test loading)
    strcpy(MAIN_FONT_PATH, global_data[2]);
    TTF_Font* test_main_font_load = TTF_OpenFont(MAIN_FONT_PATH, 1);
    if (test_main_font_load == NULL)
    {
        print_error("`load_global_resources()`: test font loading failed", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_array((void**)global_data, GLOBAL_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return;
    }
    else
    {
        TTF_CloseFont(test_main_font_load);
        test_main_font_load = NULL;
    }

    free_ptr_array((void**)global_data, GLOBAL_DATA_LINES);
    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return;
}


void _free_global_resources(void)
{
    if (ICON_TEXTURE != NULL)
    {
        SDL_DestroySurface(ICON_TEXTURE);
        ICON_TEXTURE = NULL;
    }
    
    if (NULL_TEXTURE != NULL)
    {
        SDL_DestroyTexture(NULL_TEXTURE);
        NULL_TEXTURE = NULL;
    }
}


char** read_file_by_line(const char* path, const size_t target_lines)
{
    /// REDO: This approach is very static. Use JSON.
    FILE* data_file = fopen(path, "r");
    if (data_file == NULL)
    {
        print_error("`read_file_by_line()`: couldnt't open data file", NON_SDL_ERROR);
        return NULL;
    }

    char** result = malloc(target_lines * sizeof(char*));
    if (result == NULL)
    {
        print_error("`read_file_by_line()`: couldnt't allocate memory for result object", NON_SDL_ERROR);
        fclose(data_file);
        data_file = NULL;
        return NULL;
    }
    for (size_t i = 0; i < target_lines; i++)
    {
        result[i] = malloc(64 * sizeof(char));
        if (result[i] == NULL)
        {
            print_error("`read_file_by_line()`: couldnt't allocate memory for result lines", NON_SDL_ERROR);
            for (size_t j = 0; j < i; j++)
            {
                free(result[j]);
                result[j] = NULL;
            }
            free(result);
            fclose(data_file);
            data_file = NULL;
            return (result = NULL);
        }
    }

    for (size_t i = 0; i < target_lines; i++)
    {
        if (fgets(result[i], 64, data_file) == NULL)
        {
            print_error("`read_file_by_line()`: premature EOF", NON_SDL_ERROR);
            for (size_t j = 0; j < target_lines; j++)
            {
                free(result[j]);
                result[j] = NULL;
            }
            free(result);
            fclose(data_file);
            data_file = NULL;
            return (result = NULL);
        }
        else
            result[i][strcspn(result[i], "\n")] = 0; /// `\n` sanitization
    }

    fclose(data_file);
    return result;
}

#endif /// RESOURCES_H