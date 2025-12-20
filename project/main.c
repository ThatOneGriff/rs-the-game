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

/* Audio */
#include "audio/audio.h"

/* = Library information =
SDL3 version: 3.2.28       (last updated  6.12.25)
miniaudio version: 0.11.23 (last updated 12.12.25)
nlohmann/json version: TBA

 = Compiler args (in order) =
gcc -Ofast -Wall -Wextra -Werror -std=c17 project/main.c project/audio/audio.c -o ./main.exe -lSDL3 -lSDL3_image -lSDL3_ttf -I "C:/SDL/x86_64-w64-mingw32/include" -I "C:/SDL_Image/x86_64-w64-mingw32/include" -I "C:/SDL_ttf/x86_64-w64-mingw32/include" -L "C:/SDL/x86_64-w64-mingw32/lib" -L "C:/SDL_image/x86_64-w64-mingw32/lib" -L "C:/SDL_ttf/x86_64-w64-mingw32/lib"
./main.exe
*/

/// NOTE: use texture scaling mode `SDL_SCALING_PIXELART`.


/* Predef */

int main(int argc, char *argv[]);


/* Body */

#define SDL_MAIN_HANDLED
int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    printf("\n");
    
    

    return 0;
}