/* Related header */
#include "shifting_texture.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_DestroyTexture()`.
#include <SDL3_image/SDL_image.h> /// `IMG_Load()`.

/* C headers */
#include <stdlib.h>  /// `*alloc()`.
#include <stdbool.h> /// `bool freeze_shifting`.
#include <string.h>  /// `memset()`.

/* Helper headers */
#include "../../debug.h"                   /// Error printing.
#include "../../resources.h"               /// Null texture.
#include "../../graphics/graphics_layer.h" /// `graphics_layer`.
#include "../../logic/logic_layer.h"       /// Current tick.


/* Predef */

struct Shifting_Texture init_shifting_texture(const SDL_FRect rect, const size_t max_count, const time_span_ms step, int *const exit_code);
void                    free_shifting_texture(struct Shifting_Texture *const target);
void add_to_shifting_texture(struct Shifting_Texture *const to, const char *const new_texture_path, int *const exit_code);
void render_shifting_texture(struct Shifting_Texture *const target);


/* Body */

struct Shifting_Texture init_shifting_texture(const SDL_FRect rect, const size_t max_count, const time_span_ms step, int *const exit_code)
{
    /// Object creation
    struct Shifting_Texture result = {0};
    result.rect = rect;
    result.i    = 0;
    result.step = step;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`new_shifting_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (max_count == 0)
    {
        print_error("`new_shifting_texture()`: `max_count` arg is 0. `Shifting_Texture` is not dynamic-memory", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Malloc without filling it.
    result.textures = malloc(max_count * sizeof(SDL_Texture*));
    if (result.textures == NULL)
    {
        print_error("`new_shifting_texture()`: couldn't allocate memory to texture array", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.max_count = max_count;
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_shifting_texture(struct Shifting_Texture *const target)
{
    if (target == NULL)
        return;
    
    if (target->textures != NULL)
    {
        for (size_t i = 0; i < target->cur_count; i++)
        {
            if (target->textures[i] != NULL_TEXTURE)
            {
                SDL_DestroyTexture(target->textures[i]);
                target->textures[i] = NULL;
            }
        }
        free(target->textures);
    }
    
    memset(target, 0, sizeof *target);
    target->freeze_shifting = true;
    return;
}


/* Functions */

void add_to_shifting_texture(struct Shifting_Texture *const to, const char *const new_texture_path, int *const exit_code)
{
    /// Param checking 
    if (exit_code == NULL)
        print_warning("`add_to_shifting_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (to == NULL)
    {
        print_error("`add_to_shifting_texture()`: `to` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (to->textures == NULL)
    {
        print_error("`add_to_shifting_texture()`: `to->textures` is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Check if full
    if (to->max_count == to->cur_count)
    {
        print_error("`add_to_shifting_texture()`: `to->textures` is full", NON_SDL_ERROR);
        return;
    }

    /// Insertion
    to->textures[to->cur_count] = IMG_LoadTexture(graphics_layer.renderer, new_texture_path);
    if (to->textures[to->cur_count] == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`add_to_shifting_texture()`: couldn't load the texture, replaced with null texture", IS_SDL_ERROR);
            to->textures[to->cur_count] = NULL_TEXTURE;
        }
        else
        {
            print_error("`add_to_shifting_texture()`: couldn't load the texture, and null texture is empty", IS_SDL_ERROR);
            *exit_code = EXIT_FAILURE;
            return;
        }
    }
    
    ++to->cur_count;
    *exit_code = EXIT_SUCCESS;
    return;
}


void render_shifting_texture(struct Shifting_Texture *const target)
{
    if (target == NULL || target->textures == NULL || target->cur_count == 0)
    {
        print_error("`render_shifting_texture()`: `target` or its members are invalid", NON_SDL_ERROR);
        return;
    }
    
    if      (! target->freeze_shifting && target->latest_change == 0) /// First time `latest_change` is set.
        target->latest_change = logic_layer.curr_tick;
    else if (! target->freeze_shifting && logic_layer.curr_tick - target->latest_change >= target->step)
    {
        if (++target->i == target->cur_count)
            target->i = 0;
        target->latest_change = logic_layer.curr_tick;
    }
    
    if (target->rect.x + target->rect.w <= 0 ||
        target->rect.y + target->rect.h <= 0 ||
        target->rect.x >= RENDER_WIDTH ||
        target->rect.y >= RENDER_HEIGHT)
    {
        print_warning("`render_shifting_texture()`: texture rendering out of bounds", NON_SDL_ERROR);
        return; /// While it's not an error, why waste a draw call on something not seen anyway?
    }
    SDL_RenderTexture(graphics_layer.renderer, target->textures[target->i], NULL, &target->rect);
    return;
}