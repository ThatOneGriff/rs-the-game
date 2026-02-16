Hey everyone! This is my first "big" non-tutorial project. No piece of code was written by LLM's, neither was any artwork generated.

## Notes

0) Work was done on a Windows machine. That said, Windows dependencies should be minimal: I only reckon one `#include <windows.h>` in `./project/helpers/helpers.c` for colored console output.
1) `./SDL3`, `./SDL3.dll`, `./SDL3_image.dll`, `./SDL3_ttf.dll` are all for x86_64. I know storing such files in a repository is something of a bad tone, but I wanted to avoid redownloading them, considering it's my personal project only. **If you want to download those libraries:**
	- SDL3: https://github.com/libsdl-org/SDL/releases
	- SDL3_image: https://github.com/libsdl-org/SDL_image/releases
	- SDL3_TTF: https://github.com/libsdl-org/SDL_ttf/releases
	- miniaudio: https://github.com/mackron/miniaudio/releases - header-only library, so should be platform independent.

## Compilation

1) Make sure you have all needed libraries installed (see 'Notes/1' here).
2) Make sure you have CLang & Bash on your machine.
3) Open root folder in console and **compile miniaudio:** `sh compile-miniaudio`.
4) On success, **compile the main program** with one of the following scripts:
	 - `sh compile-release` - release build with maximum optimizations;
	 - `sh compile-asan` - debug build (ASan). Strangely, conflicts with GDB on my machine, so beware;
	 - `sh compile-gdb` - debug build (GDB).

## Distribution

Run with one of the following scripts:
- `sh run` - runs the program;
- `sh run-gdb` - runs the program inside of GDB.

On running `sh pack-distribution`, you'll get a `./DISTRUBUTION` folder with copies of everything the program needs to run.