Hey everyone! This is my first "big" non-tutorial project. No piece of code was written by LLM's, neither was any artwork generated.

## Notes

0) Work was done on a Windows machine. That said, Windows dependencies should be minimal: there's only one `#include <windows.h>` in `./project/helpers/helpers.c` for colored console output, and the build instruction themselves would need to be rewritten (as they use `.dll`'s).
1) **Download the libraries.** Put the folders into `./build/SDL3/` and the `.dll` files into the root folder:
	- SDL3: https://github.com/libsdl-org/SDL/releases - `./build/SDL3/SDL/`, `./SDL3.dll`;
	- SDL3_image: https://github.com/libsdl-org/SDL_image/releases - `./build/SDL3/SDL_image/`, `./SDL3_image.dll`;
	- SDL3_TTF: https://github.com/libsdl-org/SDL_ttf/releases - `./build/SDL3/SDL_ttf/`, `./SDL3_ttf.dll`.

## Compilation

1) Make sure you have all needed libraries installed (see 'Notes/1' here).
2) Make sure you have CLang & Bash on your machine.
3) Download SDL3, SDL3_image, SDL3_TTF development packaged via the links above.
4) Open root folder in console and **compile miniaudio:** `sh compile-miniaudio`.
5) On success, **compile the main program** with one of the following scripts:
	 - `sh compile-release` - release build with maximum optimizations;
	 - `sh compile-asan` - debug build (ASan). Strangely, conflicts with GDB on my machine, so beware;
	 - `sh compile-gdb` - debug build (GDB).

## Running

Run with one of the following scripts:
- `sh run` - runs the program;
- `sh run-gdb` - runs the program inside of GDB.

## Distribution

On running `sh pack-distribution`, you'll get a `./DISTRUBUTION/` folder with copies of everything the program needs to run.