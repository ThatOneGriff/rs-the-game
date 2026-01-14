#pragma once
#ifndef MUSIC_LOADER_H
#define MUSIC_LOADER_H

#include "audio_manager.h"        /// Audio system.
#include <stdlib.h>               /// `*alloc()`.
#include "../debug.h"             /// Error printing.
#include "../deinit_stack.h"      /// Deinitialization stack.
#include "../helpers/helpers.h"   /// `free_ptr_arr()`.
#include "../helpers/random.h"    /// `randint_except()`.
#include "../logic/logic_layer.h" /// `logic_layer.curr_tick`.


/* Struct */

struct Music_Loader
{
    char** track_paths;
    size_t track_count;

    size_t       curr_track;
    time_tick_ms track_end_tick; /// Is used to control the 2s pause between tracks.
};
static struct Music_Loader music_loader; /// Singleton.


/* Predef */

void _init_music_loader(const char* music_data_path, int* exit_code);
void _free_music_loader(void);
void play_random_music (void);


/* Body */

void _init_music_loader(const char* music_data_path, int* exit_code)
{
    /// `music_loader` initial `NULL`-ing.
    music_loader.track_paths = NULL;
    music_loader.track_count = 0;
    music_loader.curr_track     = 0;
    music_loader.track_end_tick = 0; /// Temporary value to be set with the first update.

    /// Param checking
    if (exit_code == NULL)
        print_warning("`_init_music_loader()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (music_data_path == NULL)
    {
        print_error("`_init_music_loader()`: `music_data_path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// File opening
    FILE* music_data_file = fopen(music_data_path, "r");
    if (music_data_file == NULL)
    {
        print_error("`_init_music_loader()`: couldnt't open music data file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// First line indicates track amount.
    char line[100];
    fgets(line, 3, music_data_file);
    const size_t line_count = atoi(line);
    if (line_count == 0)
    {
        print_error("`_init_music_loader()`: music data line count = 0", NON_SDL_ERROR);
        fclose(music_data_file);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Deinit stack (instantiated after line count is known for certain).
    struct Deinit_Stack deinit_stack = new_deinit_stack(1 + line_count, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`_init_music_loader()`: error while creating deinit stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        fclose(music_data_file);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Memory allocation.
    music_loader.track_paths = calloc(line_count, sizeof(char*));
    if (music_loader.track_paths == NULL)
    {
        print_error("`_init_music_loader()`: couldn't allocate memory for track paths", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        fclose(music_data_file);
        *exit_code = EXIT_FAILURE;
        return;
    }
    add_to_deinit_stack(&deinit_stack, &music_loader.track_paths, (void (*)(void*))free);

    /// Path saving, test track loading.
    FILE* test_track_opener = NULL;
    for (size_t i = 0; i < line_count; i++)
    {
        /// Memory allocation
        music_loader.track_paths[i] = malloc(100 * sizeof(char));
        if (music_loader.track_paths[i] == NULL)
        {
            print_error("`_init_music_loader()`: couldn't allocate memory for a track path", NON_SDL_ERROR);
            flush_deinit_stack(&deinit_stack);
            fclose(music_data_file);
            *exit_code = EXIT_FAILURE;
            return;
        }
        add_to_deinit_stack(&deinit_stack, &music_loader.track_paths[i], (void (*)(void*))free);
        fgets(line, 100, music_data_file);

        /// Track loading
        test_track_opener = fopen(line, "r");
        if (music_data_file == NULL)
        {
            printf("(%s) ", line);
            print_warning("`_init_music_loader()`: couldnt't open track", NON_SDL_ERROR);
            continue;
        }
        else
        {
            strcpy(music_loader.track_paths[i], line);
            ++music_loader.track_count;
        }
        fclose(test_track_opener);
    }
    fclose(test_track_opener);
    test_track_opener = NULL;

    /// TODO: make the application work fine without music.
    if (music_loader.track_count == 0)
    {
        print_error("`_init_music_loader()`: no tracks have been loaded", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        fclose(music_data_file);
        *exit_code = EXIT_FAILURE;
        return;
    }

    if (music_loader.track_count < line_count)
    {
        print_warning("`_init_music_loader()`: not all tracks have been loaded", NON_SDL_ERROR);
        void* temp = realloc(music_loader.track_paths, music_loader.track_count * sizeof(char*));
        if (temp != NULL)
        {
            music_loader.track_paths = temp;
            temp = NULL;
        }
        else
        {
            print_error("`_init_music_loader()`: failed to resize `music_loader.track_paths`", NON_SDL_ERROR);
            flush_deinit_stack(&deinit_stack);
            fclose(music_data_file);
            *exit_code = EXIT_FAILURE;
            return;
        }
    }

    free_deinit_stack(&deinit_stack);
    fclose(music_data_file);
    *exit_code = EXIT_SUCCESS;
    return;
}

#endif /// MUSIC_LOADER_H