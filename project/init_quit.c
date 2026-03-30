/* Related header */
#include "init_quit.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// Initialization
#include <SDL3_image/SDL_image.h> /// of
#include <SDL3_ttf/SDL_ttf.h>     /// SDL3.

/* C headers */
#include <stdio.h>  /// I/O.

/* Root folder headers */
#include "debug.h"        /// Error / warning output.
#include "deinit_stack.h" /// Deinitialization stack.
#include "resources.h"    /// Texture & font paths.
#include "helpers/helpers.h" /// `free_ptr_arr()`.

/* Audio */
#include "_miniaudio/miniaudio.h" /// 'miniaudio' init.
#include "audio/audio_manager.h"  /// Audio manager init.
#include "audio/music_loader.h"   /// Music loading.

/* Other headers */
#include "graphics/fps.h"              /// Initialization
#include "graphics/graphics_layer.h"   /// of
#include "logic/logic_layer.h"         /// various
#include "scenes/car_manager.h"        /// game
#include "scenes/gameplay/traffic_manager.h"
#include "game_components/text/text.h" /// components.


/* Predef */

void init(int *const exit_code);
void quit(void);
void read_data(void);
void save_data(void);
[[ noreturn ]]
void program_exit(const int exit_code);


/* Body */

void init(int *const exit_code)
{
    if (exit_code == NULL)
        print_warning("`init()`: `*exit_code` arg is `NULL`");

    /// SDL3 initialization
    if (! SDL_Init(SDL_FLAGS))
    {
        print_SDL_error("`init()`: failed SDL3 initialization");
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Graphics layer
    init_graphics_layer(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: failed to init `graphics_layer`");
        SDL_Quit();
        return;
    }
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(5, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack");
        free_deinit_stack(&deinit_stack);
        SDL_Quit();
        return;
    }
    add_to_deinit_stack(&deinit_stack, graphics_layer.window,   (void (*)(void*))SDL_DestroyWindow);
    add_to_deinit_stack(&deinit_stack, graphics_layer.renderer, (void (*)(void*))SDL_DestroyRenderer);
    add_to_deinit_stack(&deinit_stack, graphics_layer.buffer,   (void (*)(void*))SDL_DestroyTexture);

    /// Logic layer
    init_logic_layer(exit_code);
    if (*exit_code == EXIT_FAILURE) /// NOTE: never happens for now.
    {
        print_error("`init()`: failed to init `logic_layer`");
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        return;
    }
    set_fps_cap(60);

    /// TTF initialization
    if (! TTF_Init())
    {
        print_SDL_error("`init()`: failed to initialize TTF");
        free_logic_layer();
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }

    load_global_resources(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: failed to load global resources");
        TTF_Quit();
        free_logic_layer();
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }
    SDL_SetWindowIcon(graphics_layer.window, ICON_TEXTURE);

    /// Car managers
    init_car_manager(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: failed to initialize car manager");
        TTF_Quit();
        free_logic_layer();
        flush_deinit_stack(&deinit_stack);
        SDL_Quit();
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Music loaders
    music_loader_gameplay = init_music_loader("./rsdt/music_gameplay.rsdt", exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_warning("`init()`: music data (gameplay) file not found");
        free_music_loader(&music_loader_gameplay);
    }
    else
        add_to_deinit_stack(&deinit_stack, &music_loader_gameplay, (void (*)(void*))free_music_loader);
    
    music_loader_menu = init_music_loader("./rsdt/music_menu.rsdt", exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_warning("`init()`: music data (menu) file not found");
        free_music_loader(&music_loader_menu);
    }
    else
        add_to_deinit_stack(&deinit_stack, &music_loader_menu, (void (*)(void*))free_music_loader);
    
    
    audio_manager.audio_is_valid = (music_loader_gameplay.valid && music_loader_menu.valid);
    
    /// Audio system
    if (audio_manager.audio_is_valid)
    {
        if (ma_engine_init(NULL, &audio_manager.engine) != MA_SUCCESS)
        {
            print_error("`init()`: failed to initialize audio engine");
            audio_manager.audio_is_valid = false;
        }
    }

    read_data();
    free_deinit_stack(&deinit_stack); /// `free` because those resources will be used.
    print_success("`init()`");
    *exit_code = EXIT_SUCCESS;
    return;
}


void read_data(void)
{
    char** save_data = read_file_by_line(SAVE_DATA_PATH, SAVE_DATA_LINES);
    if (save_data == NULL)
    {
        printf("Save file not found or empty.\n");
        return;
    }

    if (audio_manager.audio_is_valid)
        audio_manager.using_audio = (bool)    atoi(save_data[0]);
    else
        audio_manager.using_audio = false;
    curr_fps_cap_i            = (unsigned)atoi(save_data[1]);
    set_fps_cap(fps_cap_options[curr_fps_cap_i]);

    const size_t car_i = (size_t)atoi(save_data[2]);
    for (size_t i = 0; i < car_i; i++)
        get_next_car();
    
    PERSONAL_BEST =  atoi(save_data[3]);
    show_fps = (bool)atoi(save_data[4]);

    print_success("Save data read");
    free_ptr_arr((void**)save_data, SAVE_DATA_LINES);
    return;
}


void save_data(void)
{
    FILE* save_data = fopen(SAVE_DATA_PATH, "w");
    fprintf(save_data, "%d\n%u\n%llu\n%d\n%d", audio_manager.using_audio, curr_fps_cap_i, players_car_manager.cur_car, PERSONAL_BEST, show_fps);
    fclose(save_data);
    print_success("Data saved");
    return;
}


/// Works by FILO principle.
void quit(void)
{
    free_traffic_manager();
    if (audio_manager.using_audio)
    {
        free_music_loader(&music_loader_gameplay);
        free_music_loader(&music_loader_menu);
        ma_sound_uninit (&audio_manager.music);
        ma_engine_uninit(&audio_manager.engine);
    }
    free_car_manager();
    free_global_resources();
    TTF_Quit();
    free_logic_layer();
    free_graphics_layer();
    SDL_Quit();

    print_success("`quit()`");
    return;
}


[[ noreturn ]]
void program_exit(const int exit_code)
{
    if (logic_layer.save_needed) /// NOTE: a smarter system may take place, that compares initial values and the newly set ones (so that a save only happens when something was actually changed, and not changed and then set back to original).
        save_data();
    quit();
    printf("\n");
    exit(exit_code);
}