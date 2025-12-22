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


/* Predef */

int  main(int argc, char *argv[]);
void game_loop();


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
    _init_graphics_layer();
    _init_logic_layer();
    /// , because you don't want to use uninitialized `layer`'s.

    /// SDL init
    init(&exit_code);
    if (exit_code == EXIT_FAILURE)
    {
        print_error("Failure initializing the program", NON_SDL_ERROR);
        program_exit(exit_code);
    }

    game_loop();

    program_exit(EXIT_SUCCESS); /// Actual quitting of the program.
    return EXIT_SUCCESS;        /// Solely for `-Wall -Wextra` compliance.
}


void game_loop()
{
    int exit_code = EXIT_SUCCESS;
    struct Texture test_text    = create_text("Test", (SDL_Color){255,255,255,0}, rand_color(), 150, 15, &exit_code);
    struct Texture test_texture = load_texture(ICON_TEXTURE, (SDL_FRect){300,300,300,300}, &exit_code);
    struct Car car = load_car("res/data/clio-williams.rscdt", &exit_code);

    while (logic_layer.game_is_running)
    {
        SDL_RenderClear(graphics_layer.renderer);

        process_events();
        SDL_RenderTexture(graphics_layer.renderer, graphics_layer.null_texture, NULL, NULL);
        render_texture(&test_text);
        render_texture(&test_texture);
        render_texture(&car.texture);
        SDL_RenderPresent(graphics_layer.renderer);

        SDL_Delay(16);
    }

    free_car(&car);
    free_texture(&test_texture);
    free_texture(&test_text);
}