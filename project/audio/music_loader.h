#pragma once
#ifndef MUSIC_LOADER_H
#define MUSIC_LOADER_H

#include "../logic/logic_layer.h"    /// `logic_layer.curr_tick`.


/* Struct */

struct Music_Loader
{
    bool valid;
    bool active;

    char** track_paths;
    size_t track_count;

    size_t       curr_track;
    time_tick_ms track_end_tick; /// Is used to control the 2s pause between tracks.
    time_tick_ms track_start_delay;
};
extern struct Music_Loader music_loader_gameplay;
extern struct Music_Loader music_loader_menu;


/* Predef */

struct Music_Loader _init_music_loader  (const char* music_data_path, int* exit_code);
void                _freeze_music_loader(struct Music_Loader* target);
void                _free_music_loader  (struct Music_Loader* target);
void check_if_music_ended(struct Music_Loader* target);
void play_random_music   (struct Music_Loader* target);


#endif /// MUSIC_LOADER_H