/// Every header that was ever used in this project is included here.

/*- SDL -*/
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

/*- C headers -*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Main headers */
#include "debug.h"
#include "init_quit.h"
#include "resources.h"

/* Audio */
#ifdef USING_AUDIO
#include "audio/audio.h"
#endif /// USING_AUDIO

/* Game components */
#include "game_components/button.h"
#include "game_components/multi_texture.h"
#include "game_components/shifting_texture.h"
#include "game_components/texture.h"

/* Graphics */
#include "graphics/fps.h"
#include "graphics/graphics_layer.h"

/* Helpers */
#include "helpers/geometry.h"
#include "helpers/helpers.h"
#include "helpers/random.h"

/* Logic */
#include "logic/global_events.h"
#include "logic/logic_layer.h"

/* Scenes */
#include "scenes/gameplay/car.h"
#include "scenes/gameplay/gameplay_events.h"
#include "scenes/gameplay/gameplay_scene.h"

#include "scenes/menu/menu_scene.h"
#include "scenes/menu/menu_events.h"

/* Text */
#include "text/border.h"
#include "text/text.h"

/* = Library information =
SDL3 version:          3.2. 28 (last updated  6.12.25)
miniaudio version:     0.11.23 (last updated 12.12.25)
nlohmann/json version: 3.12.0  (last updated 24.12.25)
*/

/// TODO: deallocation via stack.

/* Visual TODOs: */
/// - Fix Clio Williams texture (exhaust pipe mirroring);
/// - Find an adequate font (pixel-art one may provide pixel perfection difficulties; try to find the one R.S. logo uses).

/* Structural IDEAs: */
/// - do something about the include mess;
/// ? `struct Button_Manager`
/// ? `null_*_scene()` just for error resistance

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
    
    /* Loading & preparing the scene */
    struct Gameplay_Scene gameplay_scene;
    bool gameplay_scene_opened = false; /// TEMP: just not to get an error while deinitializing BEFORE loading the gameplay scene.
    
    struct Car car = load_car("res/car_data/clio-williams.rscdt", exit_code);
    if (*exit_code == EXIT_FAILURE)
        return;
    
    struct Menu_Scene menu_scene = load_menu_scene("res/scene_data/menu.rsmsdt", exit_code);
    if (*exit_code == EXIT_FAILURE)
        return;
    logic_layer.curr_scene = &menu_scene;

    /* FPS measurement preparations */
    time_tick_ns render_start_tick   = 0; /// Temporary value.
    time_tick_ms fps_measure_1s_tick = SDL_GetTicks();
    unsigned int curr_fps = 0;
    unsigned int prev_fps = UINT_MAX;

    /* The loop */
    while (logic_layer.game_is_running)
    {
        render_start_tick = SDL_GetTicksNS();
        SDL_RenderClear(graphics_layer.renderer);
        SDL_SetRenderTarget(graphics_layer.renderer, graphics_layer.buffer);

        if (logic_layer.curr_scene == &gameplay_scene)
        {
            gameplay_scene_opened = true; /// TEMP: just not to get an error while deinitializing BEFORE loading the gameplay scene.
            process_gameplay_events(&gameplay_scene);
            gameplay_scene_tick(&gameplay_scene, exit_code);
            if (! logic_layer.remain_in_scene)
            {
                free_gameplay_scene(&gameplay_scene);
                menu_scene = load_menu_scene("res/scene_data/menu.rsmsdt", exit_code);
                if (*exit_code == EXIT_FAILURE)
                    return;
                logic_layer.curr_scene = &menu_scene;
                logic_layer.remain_in_scene = true;
            }
        }
        else if (logic_layer.curr_scene == &menu_scene)
        {
            process_menu_events(&menu_scene);
            menu_scene_tick(&menu_scene, exit_code);
            if (! logic_layer.remain_in_scene)
            {
                free_menu_scene(&menu_scene);
                gameplay_scene = load_gameplay_scene("res/scene_data/plains.rsgsdt", &car, exit_code);
                if (*exit_code == EXIT_FAILURE)
                    return;
                logic_layer.curr_scene = &gameplay_scene;
                logic_layer.remain_in_scene = true;
            }
        }
        
        SDL_SetRenderTarget(graphics_layer.renderer, NULL);
        SDL_RenderTexture(graphics_layer.renderer, graphics_layer.buffer, NULL, NULL);
        SDL_RenderPresent(graphics_layer.renderer);

        /* FPS & delay managing */
        ++curr_fps;
        FPS_manager.delta_ns = SDL_GetTicksNS() - render_start_tick;
        if (FPS_manager.fps_capped && FPS_manager.target_delta_ns > FPS_manager.delta_ns)
        {
            SDL_DelayNS(FPS_manager.target_delta_ns - FPS_manager.delta_ns);
            FPS_manager.delta_ns = FPS_manager.target_delta_ns;
        }
        
        /* FPS output */
        if (SDL_GetTicks() - fps_measure_1s_tick >= 1000) /// 1s elapsed.
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