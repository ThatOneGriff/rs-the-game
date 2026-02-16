#ifndef RESOURCES_H
#define RESOURCES_H

/* SDL3 */
#include <SDL3/SDL.h> /// SDL3.

#define GLOBAL_DATA_PATH "./rsdt/global.rsdt"
#define GLOBAL_DATA_LINES 3

extern SDL_Surface* ICON_TEXTURE;
extern SDL_Texture* NULL_TEXTURE;
extern char       MAIN_FONT_PATH[64];
extern int PERSONAL_BEST;

/// NOTE: `.rsdt` is a compromise born from a lack of time. JSON loading is preferable and will eventually be the mechanism.


/* Predef */

void load_global_resources(int *const exit_code);
void free_global_resources(void);


#endif /// RESOURCES_H