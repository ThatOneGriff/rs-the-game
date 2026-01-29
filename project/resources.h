#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H

/* SDL3 */
#include <SDL3/SDL.h> /// SDL3.

#define GLOBAL_DATA_PATH "./rsdt/global.rsdt"
#define GLOBAL_DATA_LINES 3

static SDL_Surface* ICON_TEXTURE = NULL;
static SDL_Texture* NULL_TEXTURE = NULL;
static char       MAIN_FONT_PATH[64];
static int PERSONAL_BEST = 0;

/// NOTE: `.rsdt` is a compromise born from a lack of time. JSON loading is preferable and will eventually be the mechanism.


/* Predef */

void  _load_global_resources(int* exit_code);
void  _free_global_resources(void);
char** read_file_by_line(const char* path, const size_t target_lines);

#endif /// RESOURCES_H