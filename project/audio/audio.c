/* Related headers */
#include "audio_manager.h"
#include "music_loader.h"

/* Audio */
#include "../_miniaudio/miniaudio.h" /// 'miniaudio'.

/* C headers*/
#include <stdbool.h>                 /// `bool active`.
#include <stdio.h>                   /// File read/write.
#include <stdlib.h>                  /// `*alloc()`.
#include <string.h>                  /// `memset()`.

/* Helpers */
#include "../debug.h"                /// Error printing.
#include "../deinit_stack.h"         /// Deinitialization stack.
#include "../helpers/random.h"       /// `randint_except()`.
#include "../logic/logic_layer.h"    /// `logic_layer.curr_tick`.


/* Variables */

struct Audio_Manager audio_manager        = {0};
struct Music_Loader music_loader_gameplay = {0};
struct Music_Loader music_loader_menu     = {0};


/* Predef */

struct Music_Loader init_music_loader  (const char *const music_data_path, int *const exit_code);
void                freeze_music_loader(struct Music_Loader *const target);
void                free_music_loader  (struct Music_Loader *const target);
void check_if_music_ended(struct Music_Loader *const target);
void play_random_music   (struct Music_Loader *const target);


/* Body */

struct Music_Loader init_music_loader(const char *const music_data_path, int *const exit_code)
{
    struct Music_Loader result = {0};
    result.curr_track        = ULONG_LONG_MAX;
    result.track_start_delay = 1500;
    result.latest_track_end_check_tick = logic_layer.curr_tick;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`_init_music_loader()`: `exit_code` arg is `NULL`");
    if (music_data_path == NULL)
    {
        print_error("`_init_music_loader()`: `music_data_path` arg is `NULL`");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// File opening
    FILE* music_data_file = fopen(music_data_path, "r");
    if (music_data_file == NULL)
    {
        print_error("`_init_music_loader()`: couldnt't open music data file");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// First line indicates track amount.
    char line[100];
    fgets(line, 100, music_data_file);
    const size_t line_count = (size_t)atoi(line);
    if (line_count == 0)
    {
        print_error("`_init_music_loader()`: music data line count = 0");
        fclose(music_data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Deinit stack (instantiated after line count is known for certain).
    struct Deinit_Stack deinit_stack = new_deinit_stack(line_count + 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`_init_music_loader()`: error while creating deinit stack");
        free_deinit_stack(&deinit_stack);
        fclose(music_data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Memory allocation.
    result.track_paths = calloc(line_count, sizeof(char*));
    if (result.track_paths == NULL)
    {
        print_error("`_init_music_loader()`: couldn't allocate memory for track paths");
        free_deinit_stack(&deinit_stack);
        fclose(music_data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.track_paths, (void (*)(void*))free);

    /// Path saving, test track loading.
    FILE* test_track_opener = NULL;
    for (size_t i = 0; i < line_count; i++)
    {
        /// Memory allocation
        result.track_paths[i] = malloc(100 * sizeof(char));
        if (result.track_paths[i] == NULL)
        {
            print_error("`_init_music_loader()`: couldn't allocate memory for a track path");
            flush_deinit_stack(&deinit_stack);
            fclose(music_data_file);
            *exit_code = EXIT_FAILURE;
            return result;
        }
        add_to_deinit_stack(&deinit_stack, &result.track_paths[i], (void (*)(void*))free);
        fgets(line, 100, music_data_file);
        line[strcspn(line, "\n")] = '\0';

        /// Track loading
        test_track_opener = fopen(line, "r");
        if (music_data_file == NULL)
        {
            printf("(%s) ", line);
            print_warning("`_init_music_loader()`: couldnt't open track");
            --i;
            continue;
        }
        else
        {
            strcpy(result.track_paths[i], line);
            ++result.track_count;
        }
        fclose(test_track_opener);
    }
    fclose(test_track_opener);
    test_track_opener = NULL;

    if (result.track_count == 0)
    {
        print_error("`_init_music_loader()`: no tracks have been loaded");
        flush_deinit_stack(&deinit_stack);
        fclose(music_data_file);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    if (result.track_count < line_count)
    {
        print_warning("`_init_music_loader()`: not all tracks have been loaded");
        void* temp = realloc(result.track_paths, result.track_count * sizeof(char*));
        if (temp != NULL)
        {
            result.track_paths = temp;
            temp = NULL;
        }
        else
        {
            print_error("`_init_music_loader()`: failed to resize `result.track_paths`");
            flush_deinit_stack(&deinit_stack);
            fclose(music_data_file);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }

    free_deinit_stack(&deinit_stack);
    fclose(music_data_file);
    result.valid = true;
    *exit_code = EXIT_SUCCESS;
    return result;
}


void freeze_music_loader(struct Music_Loader *const target)
{
    if (target == NULL)
    {
        print_error("`_free_music_loader()`: `target` arg is `NULL`");
        return;
    }

    target->active     = false;
    target->curr_track = ULONG_LONG_MAX;
}


void free_music_loader(struct Music_Loader *const target)
{
    if (target == NULL)
    {
        print_error("`_free_music_loader()`: `target` arg is `NULL`");
        return;
    }

    if (target->track_paths != NULL)
    {
        for (size_t i = 0; i < target->track_count; i++)
            free(target->track_paths[i]);
        free(target->track_paths);
    }
    
    memset(target, 0, sizeof *target);
    target->curr_track = ULONG_LONG_MAX;
    return;
}


void check_if_music_ended(struct Music_Loader *const target)
{
    if (target == NULL)
    {
        print_error("`check_if_music_ended()`: `target` arg is `NULL`");
        return;
    }
    if (! target->active || ! target->valid || ! audio_manager.using_audio)
        return;
    
    /// Check tick updated.
    target->latest_track_end_check_tick = logic_layer.curr_tick;
    
    /// Music hasn't ended.
    if (! ma_sound_at_end(&audio_manager.music))
        return;
    /// Music just ended.
    else if (target->track_end_tick == 0)
    {
        target->track_end_tick = logic_layer.curr_tick;
        if (target->track_start_delay == 0) /// delay = 0 => playing immediately
            play_random_music(target);
        else
            return;
    }
    /// Music ended, and delay elapsed.
    else if (logic_layer.curr_tick - target->track_end_tick >= target->track_start_delay)
        play_random_music(target);
    
    return;
}


void play_random_music(struct Music_Loader *const target)
{
    if (target == NULL)
    {
        print_error("`check_if_music_ended()`: `target` arg is `NULL`");
        return;
    }
    if (! target->valid || ! audio_manager.using_audio)
        return;
    
    target->active = true;

    if (target->track_count == 1)
    {
        ma_sound_start(&audio_manager.music);
        return;
    }

    ma_sound_uninit(&audio_manager.music);
    char         track_path[100];
    const size_t track_i = randint_except(0, (unsigned)target->track_count-1, (unsigned)target->curr_track);
    strcpy(track_path, target->track_paths[track_i]);
    if (ma_sound_init_from_file(&audio_manager.engine, track_path, 0, NULL, NULL, &audio_manager.music) != MA_SUCCESS)
    {
        printf("([%llu] %s) ", target->curr_track, track_path);
        print_warning("`play_random_music()`: couldn't play music");
        return;
    }
    target->curr_track = track_i;
    target->track_end_tick = 0;
    ma_sound_start(&audio_manager.music);
    return;
}