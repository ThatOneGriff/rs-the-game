#ifndef MUSIC_LOADER_H
#define MUSIC_LOADER_H

#include "../logic/logic_layer.h" /// `logic_layer.curr_tick`.

#define TRACK_END_CHECK_DELAY_MS 1000
#define LATEST_TRACKS_BUFFER_SIZE 5


/* Struct */

struct Music_Loader
{
    bool valid;
    bool active;

    char** track_paths;
    unsigned track_count;
    unsigned curr_track;

    unsigned latest_tracks_buffer[LATEST_TRACKS_BUFFER_SIZE];
    unsigned latest_track_i;

    time_tick_ms        track_end_tick; /// Is used to control the pause between tracks.
    time_tick_ms latest_track_end_check_tick;
    time_span_ms        track_start_delay;
};
extern struct Music_Loader music_loader_gameplay;
extern struct Music_Loader music_loader_menu;


/* Predef */

struct Music_Loader init_music_loader  (const char *const music_data_path, int *const exit_code);
void                freeze_music_loader(struct Music_Loader *const target);
void                free_music_loader  (struct Music_Loader *const target);
void check_if_music_ended(struct Music_Loader *const target);
void play_random_music   (struct Music_Loader *const target);


#endif /// MUSIC_LOADER_H