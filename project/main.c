#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "audio/audio.h"
#include <stdio.h>
#include <stdlib.h>

/* = Library information =
SDL version: 3.2.28 (last updated 06.12.2025)
nlohmann/json version: TBA

 = Compiler args (in order) =
gcc -Ofast -Wall -Werror -std=c17 project/main.c project/audio/audio.c -o ./main.exe -I "C:/SDL/x86_64-w64-mingw32/include" -I "C:/SDL_Image/x86_64-w64-mingw32/include" -I "C:/SDL_ttf/x86_64-w64-mingw32/include" -L "C:/SDL/x86_64-w64-mingw32/lib" -L "C:/SDL_image/x86_64-w64-mingw32/lib" -L "C:/SDL_ttf/x86_64-w64-mingw32/lib" -lSDL3 -lSDL3_image -lSDL3_ttf
./main.exe
*/

/// NOTE: use texture scaling mode `SDL_SCALING_PIXELART`.


#define SDL_MAIN_HANDLED
int main(int argc, char *argv[])
{
    

    return 0;
}