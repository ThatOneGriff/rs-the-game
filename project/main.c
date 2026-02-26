/*- SDL -*/
#include <SDL3/SDL.h>                /// SDL3
#include <SDL3/SDL_main.h>           /// and
#include <SDL3/SDL_timer.h>          /// its parts.

/*- C headers -*/
#include <stdio.h>                   /// `printf('\n')`.

/* Root folder headers */
#include "debug.h"                   /// Error printing.
#include "init_quit.h"               /// Program initialization.

/* Audio */
#include "audio/audio_manager.h"     /// Audio system.
#include "audio/music_loader.h"      /// Music loading.

/* Graphics */
#include "graphics/fps.h"            /// FPS manager.
#include "graphics/graphics_layer.h" /// Graphics layer.

/* Logic */
#include "logic/logic_layer.h"       /// Logic layer.s

/* Scenes */
#include "scenes/car_manager.h"              /// Car manager.
#include "scenes/gameplay/gameplay_events.h" /// Gameplay events & input.
#include "scenes/gameplay/gameplay_scene.h"  /// Gameplay scene.
#include "scenes/menu/menu_events.h"         /// Menu events & input.
#include "scenes/menu/menu_scene.h"          /// Menu scene.

/* = Library information =
SDL3          version: 3.4.0   (last checked 17.02.26) | https://github.com/libsdl-org/SDL/releases
SDL3_image    version: 3.4.0   (last updated 17.02.26) | https://github.com/libsdl-org/SDL_image/releases
SDL3_ttf      version: 3.2.2   (last checked 17.02.26) | https://github.com/libsdl-org/SDL_ttf/releases
miniaudio     version: 0.11.24 (last updated 17.02.26) | https://github.com/mackron/miniaudio/releases
nlohmann/json version: 3.12.0  (last checked 17.02.26) | https://github.com/nlohmann/json/releases [UNUSED]
*/

/* TODOs: */
/// - FPS output into the window, fully disable console on release builds;
/// - optional logs;
/// - pushing an array of points/textures into array-like members of `game_components/`, instead of individual `add_*()` calls;
/// - player's car bouncing in beat to the music.

/* IDEAs: */
/// - progression system;
/// - switch between metric and imperial system;
/// - mouse control;
/// - click sound (thus an option to disable music and/or sounds, not just the audio).

/* Predef */

[[ noreturn ]] /// Reason: `program_exit` at the end handles quitting.
int  main(const int argc, char* argv[]);
void game_loop(int *const exit_code);


/* Body */

[[ noreturn ]] /// Reason: `program_exit` at the end handles quitting.
int main(const int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    printf("\n");
    /// [!!!] STRICTLY ON TOP [!!!]
    int exit_code = EXIT_SUCCESS;
    SDL_SetMainReady();
    init(&exit_code);
    if (exit_code == EXIT_FAILURE)
    {
        print_error("Failure initializing the program", NON_SDL_ERROR);
        program_exit(exit_code);
    }
    /// , because you don't want to work uninitialized.

    game_loop(&exit_code);

    program_exit(exit_code); /// Actual quitting of the program.
}


void game_loop(int *const exit_code)
{
    if (exit_code == NULL)
        print_warning("`game_loop()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Loading & preparing the scene
    
    load_menu_scene(get_curr_car(), exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`game_loop()`: failed to load `menu_scene`", NON_SDL_ERROR);
        return;
    }
    logic_layer.curr_scene = &menu_scene;

    /// FPS measurement preparations
    time_tick_ns render_start_tick   = SDL_GetTicksNS();
    time_tick_ms fps_measure_1s_tick = SDL_GetTicks();
    unsigned int curr_fps = 0;
    UNUSED(curr_fps); /// TEMP

    if (audio_manager.using_audio && audio_manager.audio_is_valid)
        play_random_music(&music_loader_menu);
    
    /// The loop
    while (logic_layer.game_is_running)
    {
        /// Preparations
        logic_layer.curr_tick = SDL_GetTicks() - logic_layer.real_tick_diff;
        SDL_RenderClear(graphics_layer.renderer);
        SDL_SetRenderTarget(graphics_layer.renderer, graphics_layer.buffer);

        /// Gameplay scene processing
        if (logic_layer.curr_scene == &gameplay_scene)
        {
            if (audio_manager.using_audio
             && logic_layer.curr_tick >= music_loader_gameplay.latest_track_end_check_tick + TRACK_END_CHECK_DELAY_MS)
                check_if_music_ended(&music_loader_gameplay);
            
            process_gameplay_events();
            /// Gameplay scene ignores `graphics_layer.screen_changed`, as it's dynamic.
            if (! gameplay_scene.pause_screen.is_open)
            {
                render_gameplay_scene();
                ++curr_fps;
            }
            /// Pause screen abides to `graphics_layer.screen_changed`.
            else if (gameplay_scene.pause_screen.is_open && (graphics_layer.screen_changed || graphics_layer.force_render))
            {
                render_pause_screen(&gameplay_scene.pause_screen);
                ++curr_fps;
                graphics_layer.screen_changed = false;
                graphics_layer.force_render   = false;
            }

            /// Scene switch (to menu)
            if (! logic_layer.remain_in_scene)
            {
                free_gameplay_scene();
                load_menu_scene(get_curr_car(), exit_code);
                if (*exit_code == EXIT_FAILURE)
                    return;
                logic_layer.curr_scene = &menu_scene;
                logic_layer.remain_in_scene = true;
                logic_layer.real_tick_diff  = 0;
                if (audio_manager.using_audio)
                {
                    freeze_music_loader(&music_loader_gameplay);
                    play_random_music  (&music_loader_menu);
                }
            }
        }
        /// Menu scene processing
        else if (logic_layer.curr_scene == &menu_scene)
        {
            if (audio_manager.using_audio
             && logic_layer.curr_tick >= music_loader_menu.latest_track_end_check_tick + TRACK_END_CHECK_DELAY_MS)
                check_if_music_ended(&music_loader_menu);
            
            process_menu_events();
            if (graphics_layer.screen_changed || graphics_layer.force_render) /// 2nd part of the equation for the initial render to happen.
            {
                render_menu_scene();
                ++curr_fps;
                graphics_layer.screen_changed = false;
                graphics_layer.force_render   = false;
            }
            
            /// Scene switch
            if (! logic_layer.remain_in_scene)
            {
                free_menu_scene();
                load_gameplay_scene("./rsdt/scene_data/plains.rsdt", get_curr_car(), exit_code);
                if (*exit_code == EXIT_FAILURE)
                    return;
                logic_layer.curr_scene = &gameplay_scene;
                logic_layer.remain_in_scene = true;
                if (audio_manager.using_audio)
                {
                    freeze_music_loader(&music_loader_menu);
                    play_random_music  (&music_loader_gameplay);
                }
            }
        }
        
        /// Rendering
        SDL_SetRenderTarget(graphics_layer.renderer, NULL);
        SDL_RenderTexture  (graphics_layer.renderer, graphics_layer.buffer, NULL, NULL);
        SDL_RenderPresent  (graphics_layer.renderer);

        /// FPS & delay management
        FPS_manager.delta_ns = SDL_GetTicksNS() - render_start_tick;
        if (FPS_manager.fps_capped && FPS_manager.target_delta_ns > FPS_manager.delta_ns)
        {
            SDL_DelayNS(FPS_manager.target_delta_ns - FPS_manager.delta_ns);
            FPS_manager.delta_ns = FPS_manager.target_delta_ns;
        }
        render_start_tick = SDL_GetTicksNS();
        
        /// FPS output
        if (SDL_GetTicks() - fps_measure_1s_tick >= 1000) /// 1s since last measurement elapsed.
        {
            fps_measure_1s_tick = SDL_GetTicks();
            curr_fps = 0;
        }
    }

    if      (logic_layer.curr_scene == &gameplay_scene)
        free_gameplay_scene();
    else if (logic_layer.curr_scene == &menu_scene)
        free_menu_scene();
    logic_layer.curr_scene = NULL;
    
    *exit_code = EXIT_SUCCESS;
    return;
}