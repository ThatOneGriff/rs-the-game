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

/* Graphics */
#include "graphics/fps.h"
#include "graphics/graphics_layer.h"
#include "graphics/multi_texture.h"
#include "graphics/texture.h"

/* Helpers */
#include "helpers/geometry.h"
#include "helpers/helpers.h"
#include "helpers/random.h"

/* Logic */
#include "logic/input.h"
#include "logic/logic_layer.h"

/* Scenes */
#include "scenes/gameplay/car.h"
#include "scenes/gameplay/gameplay_controls.h"
#include "scenes/gameplay/gameplay_scene.h"

/* Text */
#include "text/border.h"
#include "text/text.h"

/* = Library information =
SDL3 version: 3.2.28       (last updated  6.12.25)
miniaudio version: 0.11.23 (last updated 12.12.25)
nlohmann/json version: TBA

 = Compiler args (1: w/ audio, 2: w/o audio) =
gcc -Ofast -Wall -Wextra -Werror -Wno-discarded-qualifiers -std=c17 project/main.c project/audio/audio.c -o ./main.exe -lSDL3 -lSDL3_image -lSDL3_ttf -I "C:/SDL/x86_64-w64-mingw32/include" -I "C:/SDL_Image/x86_64-w64-mingw32/include" -I "C:/SDL_ttf/x86_64-w64-mingw32/include" -L "C:/SDL/x86_64-w64-mingw32/lib" -L "C:/SDL_image/x86_64-w64-mingw32/lib" -L "C:/SDL_ttf/x86_64-w64-mingw32/lib"
gcc -Ofast -Wall -Wextra -Werror -Wno-discarded-qualifiers -std=c17 project/main.c -o ./main.exe -lSDL3 -lSDL3_image -lSDL3_ttf -I "C:/SDL/x86_64-w64-mingw32/include" -I "C:/SDL_Image/x86_64-w64-mingw32/include" -I "C:/SDL_ttf/x86_64-w64-mingw32/include" -L "C:/SDL/x86_64-w64-mingw32/lib" -L "C:/SDL_image/x86_64-w64-mingw32/lib" -L "C:/SDL_ttf/x86_64-w64-mingw32/lib"
./main.exe
*/

/// REDO: error resource un-allocation almost never follows FILO pattern. As `miniaudio` has shown, this may turn out in an error.


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
    struct Gameplay_Scene scene = load_gameplay_scene("res/images/environment/sky.png",
                                                      "res/images/environment/ground.png",
                                                      "res/images/environment/tree.png",
                                                      exit_code);
    if (*exit_code == EXIT_FAILURE)
        return;
    struct Car car = load_car("res/car_data/clio-williams.rscdt", exit_code);
    if (*exit_code == EXIT_FAILURE)
        return;

    /* FPS measurement preparations */
    unsigned long long int render_start_time = 0;
    unsigned int curr_fps = 0;
    unsigned int prev_fps = UINT_MAX;
    unsigned int fps_render_start_tick = SDL_GetTicks();

    /* The loop */
    while (logic_layer.game_is_running)
    {
        render_start_time = SDL_GetTicksNS();
        SDL_RenderClear(graphics_layer.renderer);
        SDL_SetRenderTarget(graphics_layer.renderer, graphics_layer.buffer);

        process_global_events();
        process_gameplay_input(&car);
        render_gameplay_scene(&scene, &car, exit_code);
        if (*exit_code == EXIT_FAILURE)
            return;
        
        SDL_SetRenderTarget(graphics_layer.renderer, NULL);
        SDL_RenderTexture(graphics_layer.renderer, graphics_layer.buffer, NULL, NULL);
        SDL_RenderPresent(graphics_layer.renderer);

        /* FPS & delay managing */
        ++curr_fps;
        FPS_manager.delta_ns = SDL_GetTicksNS() - render_start_time;
        if (FPS_manager.fps_capped && FPS_manager.target_delta_ns > FPS_manager.delta_ns)
        {
            SDL_DelayNS(FPS_manager.target_delta_ns - FPS_manager.delta_ns);
            FPS_manager.delta_ns = FPS_manager.target_delta_ns;
        }
        
        /* FPS output */
        if (SDL_GetTicks() - fps_render_start_tick >= 1000) /// 1s elapsed.
        {
            print_compare_fps(curr_fps, prev_fps);
            fps_render_start_tick = SDL_GetTicks();
            prev_fps = curr_fps;
            curr_fps = 0;
        } 
    }

    free_gameplay_scene(&scene);
    *exit_code = EXIT_SUCCESS;
}