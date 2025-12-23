/// Every header that was ever used in this project is included here.

/*- SDL -*/
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

/*- C headers -*/
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
#include "helpers/helpers.h"
#include "helpers/random.h"

/* Logic */
#include "logic/input.h"
#include "logic/logic_layer.h"

/* Scenes */
#include "scenes/gameplay/car.h" /// For test; will include the rest later.

/* Text */
#include "text/border.h"
#include "text/text.h"

/* = Library information =
SDL3 version: 3.2.28       (last updated  6.12.25)
miniaudio version: 0.11.23 (last updated 12.12.25)
nlohmann/json version: TBA

 = Compiler args (1: w/ audio, 2: w/o audio) =
gcc -Ofast -Wall -Wextra -Werror -std=c17 project/main.c project/audio/audio.c -o ./main.exe -lSDL3 -lSDL3_image -lSDL3_ttf -I "C:/SDL/x86_64-w64-mingw32/include" -I "C:/SDL_Image/x86_64-w64-mingw32/include" -I "C:/SDL_ttf/x86_64-w64-mingw32/include" -L "C:/SDL/x86_64-w64-mingw32/lib" -L "C:/SDL_image/x86_64-w64-mingw32/lib" -L "C:/SDL_ttf/x86_64-w64-mingw32/lib"
gcc -Ofast -Wall -Wextra -Werror -std=c17 project/main.c -o ./main.exe -lSDL3 -lSDL3_image -lSDL3_ttf -I "C:/SDL/x86_64-w64-mingw32/include" -I "C:/SDL_Image/x86_64-w64-mingw32/include" -I "C:/SDL_ttf/x86_64-w64-mingw32/include" -L "C:/SDL/x86_64-w64-mingw32/lib" -L "C:/SDL_image/x86_64-w64-mingw32/lib" -L "C:/SDL_ttf/x86_64-w64-mingw32/lib"
./main.exe
*/

/// NOTE: use texture scaling mode `SDL_SCALING_PIXELART`.
/// REDO: error resource un-allocation almost never follows FILO pattern. As `miniaudio` has shown, this is bad.


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
    /* TEMP: test textures & structs */
    struct Texture test_text    = create_text("Test", (SDL_Color){255,255,255,0}, rand_color(), 150, 15, exit_code);
    struct Texture test_texture = load_texture(ICON_TEXTURE, (SDL_FRect){300,300,300,300}, exit_code);
    struct Car car = load_car("res/car_data/clio-williams.rscdt", exit_code);
    if (*exit_code == EXIT_FAILURE)
        return;

    /* The action */
    unsigned long long int render_start_time = 0;
    unsigned long long int delay_ns = 0;

    unsigned int curr_fps = 0;
    unsigned int prev_fps = 0;
    unsigned int fps_render_start_tick = SDL_GetTicks();
    while (logic_layer.game_is_running)
    {
        render_start_time = SDL_GetTicksNS();
        SDL_RenderClear(graphics_layer.renderer);
        SDL_SetRenderTarget(graphics_layer.renderer, graphics_layer.buffer);

        process_events();
        SDL_RenderTexture(graphics_layer.renderer, graphics_layer.null_texture, NULL, NULL); /// TEMP background
        render_texture(&test_text);
        render_texture(&test_texture);
        render_texture(&car.texture);
        
        SDL_SetRenderTarget(graphics_layer.renderer, NULL);
        SDL_RenderTexture(graphics_layer.renderer, graphics_layer.buffer, NULL, NULL);
        SDL_RenderPresent(graphics_layer.renderer);

        /* FPS output */
        ++curr_fps;
        if (SDL_GetTicks() - fps_render_start_tick >= 1000) /// 1s elapsed.
        {
            if (curr_fps == prev_fps || prev_fps == 0)
            {
                textcolor(GRAY);
                printf("[~]");
                textcolor(WHITE);
            }
            else if (curr_fps > prev_fps)
            {
                textcolor(GREEN);
                printf("[+]");
                textcolor(WHITE);
            }
            else if (curr_fps < prev_fps)
            {
                textcolor(RED);
                printf("[-]");
                textcolor(WHITE);
            }
            printf(" %u FPS\n", curr_fps);
            fps_render_start_tick = SDL_GetTicks();
            prev_fps = curr_fps;
            curr_fps = 0;
        } 

        /* FPS & delay managing */
        FPS_manager.delta_ns = SDL_GetTicksNS() - render_start_time;
        if (! FPS_manager.fps_capped)
            continue;
        
        if (FPS_manager.delta_ns > FPS_manager.target_delta_ns)
        {
            FPS_manager.lag_compensation_ns += FPS_manager.delta_ns - FPS_manager.target_delta_ns;
            continue; /// No need to wait if we're already over target delta.
        }

        delay_ns = FPS_manager.target_delta_ns - FPS_manager.delta_ns;
        if (delay_ns > FPS_manager.lag_compensation_ns)
        {
            delay_ns -= FPS_manager.lag_compensation_ns;
            FPS_manager.lag_compensation_ns = 0;
        }
        else
        {
            FPS_manager.lag_compensation_ns -= delay_ns;
            delay_ns = 0;
        }
        if (delay_ns != 0)
            SDL_DelayNS(delay_ns);
    }

    free_car(&car);
    free_texture(&test_texture);
    free_texture(&test_text);
    *exit_code = EXIT_SUCCESS;
}