/* Related header */
#include "traffic_car.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL things.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* C headers */
#include <stdlib.h> /// Exit codes.
#include <string.h> /// `memset()`.

/* Helper headers */
#include "../../debug.h"             /// Error printing.
#include "../../deinit_stack.h"      /// Deinitialization stack.
#include "../../resources.h"         /// File reading & null texture.
#include "../../graphics/graphics_layer.h" /// Graphics layer.
#include "../../helpers/helpers.h"   /// `free_ptr_arr()`.
//#include "../../helpers/geometry.h"  /// `center_x()`.
#include "../../logic/logic_layer.h" /// `time_tick_ms`.


/* Predef */

struct Traffic_Car load_traffic_car(const char *const path, int *const exit_code);
void               free_traffic_car(struct Traffic_Car *const target);
void             render_traffic_car(struct Traffic_Car *const target);


/* Body */


struct Traffic_Car load_traffic_car(const char *const path, int *const exit_code)
{
    /// Zero-filling
    struct Traffic_Car result = {0};
    result.base_texture = 2;
    result.lane_id = ULONG_LONG_MAX;
    result.path_pt = ULONG_LONG_MAX;

    /// Checking params
    if (exit_code == NULL)
        print_warning("`load_traffic_car()`: `exit_code` arg is `NULL`");
    if (path == NULL)
    {
        print_error("`load_traffic_car()`: `path` arg is `NULL`");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Reading data
    char** car_data = read_file_by_line(path, TRAFFIC_CAR_DATA_LINES);
    if (car_data == NULL)
    {
        print_error("`load_traffic_car()`: couldn't read the contents of car data file");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(3, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_traffic_car()`: couldn't instance a deinitialization stack");
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)car_data, TRAFFIC_CAR_DATA_LINES);
        return result;
    }

    strcpy(result.name, car_data[0]);
    #define LINES_BEFORE_TEXTURES 1

    /// Textures (gameplay)
    for (size_t i = 0; i < 3; i++)
    {
        result.textures[i] = IMG_LoadTexture(graphics_layer.renderer, car_data[i + LINES_BEFORE_TEXTURES]);
        if (result.textures[i] == NULL)
        {
            if (NULL_TEXTURE != NULL)
            {
                print_SDL_warning("`load_traffic_car()`: couldn't load texture, replaced with null texture");
                result.textures[i] = NULL_TEXTURE;
            }
            else
            {
                print_SDL_error("`load_traffic_car()`: couldn't load texture, and null texture is empty");
                flush_deinit_stack(&deinit_stack);
                free_ptr_arr((void**)car_data, TRAFFIC_CAR_DATA_LINES);
                *exit_code = EXIT_FAILURE;
                return result;
            }
        }
        else
            add_to_deinit_stack(&deinit_stack, result.textures[i], (void (*)(void*))SDL_DestroyTexture);
    }
    
    free_ptr_arr((void**)car_data, TRAFFIC_CAR_DATA_LINES);
    free_deinit_stack(&deinit_stack);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_traffic_car(struct Traffic_Car *const target)
{
    if (target == NULL)
        return;
    
    for (size_t i = 0; i < 3; i++)
    {
        if (target->textures[i] != NULL_TEXTURE)
        {
            SDL_DestroyTexture(target->textures[i]);
            target->textures[i] = NULL;
        }
    }
    
    memset(target, 0, sizeof *target);
    return;
}


void render_traffic_car(struct Traffic_Car *const target)
{
    if (target == NULL) /// TODO: check for everything.
    {
        print_error("`render_traffic_car()`: `target` arg is `NULL`");
        return;
    }

    SDL_RenderTexture(graphics_layer.renderer, target->textures[target->base_texture], NULL, &target->coords);
    return;
}