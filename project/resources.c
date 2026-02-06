/* Related header */
#include "resources.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.
#include <SDL3_ttf/SDL_ttf.h>     /// SDL3_ttf.

/* C headers */
#include <stdio.h>  /// `strcpy()`
#include <stdlib.h> /// `*alloc()`.

/* Root folder headers */
#include "debug.h"        /// Error printing.
#include "deinit_stack.h" /// Deinitialization.

/* Other headers */
#include "graphics/graphics_layer.h" /// `graphics_layer.renderer` for texture loading.
#include "helpers/helpers.h"         /// `free_ptr_arr()`.


SDL_Surface* ICON_TEXTURE = NULL;
SDL_Texture* NULL_TEXTURE = NULL;
char       MAIN_FONT_PATH[64];
int PERSONAL_BEST = 0;


/* Body */

/// Must be called after SDL, TTF, & `renderer` have been initialized.
/// Called in `init()` or upon resource reloading.
void load_global_resources(int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`load_global_resources()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);

    /// Reading global data file
    char** global_data = read_file_by_line(GLOBAL_DATA_PATH, GLOBAL_DATA_LINES);
    if (global_data == NULL)
    {
        print_error("`load_global_resources()`: couldn't read global data", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Icon
    ICON_TEXTURE = IMG_Load(global_data[0]);
    if (ICON_TEXTURE == NULL)
    {
        print_error("`load_global_resources()`: couldn't load app icon", IS_SDL_ERROR);
        free_ptr_arr((void**)global_data, GLOBAL_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return;
    }
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(2, exit_code); /// Not adding the last element (font loading). Also, `global_data` needs its own treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_global_resources()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return;
    }
    add_to_deinit_stack(&deinit_stack, ICON_TEXTURE, (void (*)(void*))SDL_DestroySurface);

    /// Null texture
    NULL_TEXTURE = IMG_LoadTexture(graphics_layer.renderer, global_data[1]);
    if (NULL_TEXTURE == NULL)
        print_warning("`load_global_resources()`: couldn't load null texture (not critical)", IS_SDL_ERROR);
    else
        add_to_deinit_stack(&deinit_stack, NULL_TEXTURE, (void (*)(void*))SDL_DestroyTexture);

    /// Font (+ test loading)
    strcpy(MAIN_FONT_PATH, global_data[2]);
    TTF_Font* test_main_font_load = TTF_OpenFont(MAIN_FONT_PATH, 1);
    if (test_main_font_load == NULL)
    {
        print_error("`load_global_resources()`: test font loading failed", IS_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)global_data, GLOBAL_DATA_LINES);
        *exit_code = EXIT_FAILURE;
        return;
    }
    else
    {
        TTF_CloseFont(test_main_font_load);
        test_main_font_load = NULL;
    }

    free_ptr_arr((void**)global_data, GLOBAL_DATA_LINES);
    free_deinit_stack(&deinit_stack); /// `free` because those resources will be used.
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_global_resources(void)
{
    if (ICON_TEXTURE != NULL)
    {
        SDL_DestroySurface(ICON_TEXTURE);
        ICON_TEXTURE = NULL;
    }
    
    if (NULL_TEXTURE != NULL)
    {
        SDL_DestroyTexture(NULL_TEXTURE);
        NULL_TEXTURE = NULL;
    }
}