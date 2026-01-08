/*- SDL -*/
#include <SDL3/SDL.h>                /// SDL3
#include <SDL3/SDL_main.h>           /// and
#include <SDL3/SDL_timer.h>          /// its parts.

/*- C headers -*/
#include <stdbool.h>                 /// `bool gameplay_scene_opened` (TEMP).
#include <stdio.h>                   /// `printf('\n')`.

/* Root folder headers */
#include "debug.h"                   /// Error printing.
#include "init_quit.h"               /// Program initialization.

/* Audio */
#ifdef USING_AUDIO
#include "audio/audio.h"             /// Wrapper for 'miniaudio'.
#endif /// USING_AUDIO

/* Graphics */
#include "graphics/fps.h"            /// FPS manager.
#include "graphics/graphics_layer.h" /// Graphics layer.

/* Logic */
#include "logic/logic_layer.h"       /// Logic layer.s

/* Scenes */
#include "scenes/gameplay/car.h"             /// Car.
#include "scenes/gameplay/gameplay_events.h" /// Gameplay events & input.
#include "scenes/gameplay/gameplay_scene.h"  /// Gameplay scene.
#include "scenes/menu/menu_events.h"         /// Menu events & input.
#include "scenes/menu/menu_scene.h"          /// Menu scene.

/* = Library information =
SDL3 version:          3.4.0   (last updated  7.01.26)
SDL3_image version:    3.2.6   (last updated  7.01.26)
SDL3_ttf version:      3.2.2   (last checked  7.01.26)
miniaudio version:     0.11.23 (last checked  7.01.26)
nlohmann/json version: 3.12.0  (last updated 24.12.25)
*/

/* TODOs: */
/// - adding an array of points/textures to array-like members of `game_components`, instead of individual `add_*()`'s.

/* Visual TODOs: */
/// - Moving trees;
/// - `Shifting_Texture` for road;
/// - Moving clouds (+ a clear sky texture then);
/// - photograph some cars in Forza for traffic;
/// - fix Clio Williams half-left texture (exhaust pipe mirrored wrong);
/// - player's car bouncing in beat to the music.

/* Structural IDEAs: */
/// ? `null_move_component` and similar things for each `game_components`', for code clarity;
/// ? `struct Button_Manager`;
/// ? A system of rendering layers.


/* Predef */

int  main(int argc, char *argv[]);
void game_loop(int* exit_code);


/* Body */

#define SDL_MAIN_HANDLED
int main(int argc, char *argv[])
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
    return exit_code;        /// Solely for `-Wall -Wextra` compliance.
}


void game_loop(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`game_loop()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Loading & preparing the scene
    struct Gameplay_Scene gameplay_scene;
    bool gameplay_scene_opened = false; /// TEMP: just not to get an error while deinitializing BEFORE loading the gameplay scene.
    
    struct Car car = load_car("./rsdt/car_data/clio-williams.rsdt", exit_code);
    if (*exit_code == EXIT_FAILURE)
        return;
    
    struct Menu_Scene menu_scene = load_menu_scene(exit_code);
    if (*exit_code == EXIT_FAILURE)
        return;
    logic_layer.curr_scene = &menu_scene;

    /// FPS measurement preparations
    time_tick_ns render_start_tick   = SDL_GetTicksNS();
    time_tick_ms fps_measure_1s_tick = SDL_GetTicks();
    unsigned int curr_fps = 0;
    unsigned int prev_fps = UINT_MAX;

    /// The loop
    while (logic_layer.game_is_running)
    {
        /// Preparations
        logic_layer.curr_tick = SDL_GetTicks();
        SDL_RenderClear(graphics_layer.renderer);
        SDL_SetRenderTarget(graphics_layer.renderer, graphics_layer.buffer);

        /// Gameplay scene processing
        if (logic_layer.curr_scene == &gameplay_scene)
        {
            gameplay_scene_opened = true; /// TEMP: just not to get an error while deinitializing BEFORE loading the gameplay scene.
            process_gameplay_events(&gameplay_scene);
            gameplay_scene_tick(&gameplay_scene, exit_code);
            /// Scene switch
            if (! logic_layer.remain_in_scene)
            {
                free_gameplay_scene(&gameplay_scene);
                menu_scene = load_menu_scene(exit_code);
                if (*exit_code == EXIT_FAILURE)
                    return;
                logic_layer.curr_scene = &menu_scene;
                logic_layer.remain_in_scene = true;
            }
        }
        /// Menu scene processing
        else if (logic_layer.curr_scene == &menu_scene)
        {
            process_menu_events(&menu_scene);
            menu_scene_tick(&menu_scene, exit_code);
            /// Scene switch
            if (! logic_layer.remain_in_scene)
            {
                free_menu_scene(&menu_scene);
                gameplay_scene = load_gameplay_scene("./rsdt/scene_data/plains.rsdt", &car, exit_code);
                if (*exit_code == EXIT_FAILURE)
                    return;
                logic_layer.curr_scene = &gameplay_scene;
                logic_layer.remain_in_scene = true;
            }
        }
        
        /// Rendering
        SDL_SetRenderTarget(graphics_layer.renderer, NULL);
        SDL_RenderTexture  (graphics_layer.renderer, graphics_layer.buffer, NULL, NULL);
        SDL_RenderPresent  (graphics_layer.renderer);

        /// FPS & delay management
        ++curr_fps;
        FPS_manager.delta_ns = SDL_GetTicksNS() - render_start_tick;
        if (FPS_manager.fps_capped && FPS_manager.target_delta_ns > FPS_manager.delta_ns)
        {
            SDL_DelayNS(FPS_manager.target_delta_ns - FPS_manager.delta_ns - FPS_manager.delta_ns/40.0);
            FPS_manager.delta_ns = FPS_manager.target_delta_ns;
        }
        render_start_tick = SDL_GetTicksNS();
        
        /// FPS output
        if (SDL_GetTicks() - fps_measure_1s_tick >= 1000) /// 1s since last measurement elapsed.
        {
            print_compare_fps(curr_fps, prev_fps);
            fps_measure_1s_tick = SDL_GetTicks();
            prev_fps = curr_fps;
            curr_fps = 0;
        } 
    }

    if (gameplay_scene_opened)
        free_gameplay_scene(&gameplay_scene);
    free_menu_scene(&menu_scene);
    *exit_code = EXIT_SUCCESS;
}