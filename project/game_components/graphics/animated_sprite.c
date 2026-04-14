/* Related header */
#include "animated_sprite.h"

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

struct Animated_Sprite init_animated_sprite(const SDL_FRect rect, const unsigned short max_count, const time_span_ms step, int *const exit_code);
void                   free_animated_sprite(struct Animated_Sprite *const target);
void add_to_animated_sprite(struct Animated_Sprite *const to, const char *const new_texture_path, int *const exit_code);
void render_animated_sprite(struct Animated_Sprite *const target);


/* Body */

struct Animated_Sprite init_animated_sprite(const SDL_FRect rect, const unsigned short max_count, const time_span_ms step, int *const exit_code)
{
    /// Object creation
    struct Animated_Sprite result = {0};
    result.rect = rect;
    result.i    = 0;
    result.step = step;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`new_animated_sprite()`: `exit_code` arg is `NULL`");
    if (max_count == 0)
    {
        print_error("`new_animated_sprite()`: `max_count` arg is 0. `Shifting_Texture` is not dynamic-memory");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Malloc without filling it.
    result.frames = malloc(max_count * sizeof(SDL_Texture*));
    if (result.frames == NULL)
    {
        print_error("`new_animated_sprite()`: couldn't allocate memory to texture array");
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.max_count = max_count;
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_animated_sprite(struct Animated_Sprite *const target)
{
    if (target == NULL)
        return;
    
    if (target->frames != NULL)
    {
        for (unsigned short i = 0; i < target->cur_count; i++)
        {
            if (target->frames[i] != NULL_TEXTURE)
            {
                SDL_DestroyTexture(target->frames[i]);
                target->frames[i] = NULL;
            }
        }
        free(target->frames);
    }
    
    memset(target, 0, sizeof *target);
    target->freeze_shifting = true;
    return;
}


/* Functions */

void add_to_animated_sprite(struct Animated_Sprite *const to, const char *const new_texture_path, int *const exit_code)
{
    /// Param checking 
    if (exit_code == NULL)
        print_warning("`add_to_animated_sprite()`: `exit_code` arg is `NULL`");
    if (to == NULL)
    {
        print_error("`add_to_animated_sprite()`: `to` arg is `NULL`");
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (to->frames == NULL)
    {
        print_error("`add_to_animated_sprite()`: `to->frames` is `NULL`");
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Check if full
    if (to->max_count == to->cur_count)
    {
        print_error("`add_to_animated_sprite()`: `to->frames` is full");
        return;
    }

    /// Insertion
    to->frames[to->cur_count] = IMG_LoadTexture(graphics_layer.renderer, new_texture_path);
    if (to->frames[to->cur_count] == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_SDL_warning("`add_to_animated_sprite()`: couldn't load the texture, replaced with null texture");
            to->frames[to->cur_count] = NULL_TEXTURE;
        }
        else
        {
            print_SDL_error("`add_to_animated_sprite()`: couldn't load the texture, and null texture is empty");
            *exit_code = EXIT_FAILURE;
            return;
        }
    }
    
    ++to->cur_count;
    *exit_code = EXIT_SUCCESS;
    return;
}


void render_animated_sprite(struct Animated_Sprite *const target)
{
    if (target == NULL || target->frames == NULL || target->cur_count == 0)
    {
        print_error("`render_animated_sprite()`: `target` or its members are invalid");
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
        print_warning("`render_animated_sprite()`: texture rendering out of bounds");
        return; /// While it's not an error, why waste a draw call on something not seen anyway?
    }
    SDL_RenderTexture(graphics_layer.renderer, target->frames[target->i], NULL, &target->rect);
    return;
}