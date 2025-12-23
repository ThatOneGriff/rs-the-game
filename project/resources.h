#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H

#include <stdlib.h> /// `malloc`
#include "debug.h"  /// Error printing.

#define ICON_TEXTURE "res/images/icon.png"
#define NULL_TEXTURE "res/images/null.png"
#define MAIN_FONT    "res/fonts/MysteryQuest.ttf"

#define RSCDT_LINES 7


/* Body */

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