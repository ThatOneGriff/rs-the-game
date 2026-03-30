/* Related header */
#include "multi_sprite.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_DestroyTexture()`.
#include <SDL3_image/SDL_image.h> /// `IMG_Load()`.

/* C headers */
#include <stdlib.h>               /// `*alloc()`.
#include <string.h>               /// `memset()`.

/* Helpers */
#include "../../debug.h"                   /// Error printing.
#include "../../resources.h"               /// Null texture.
#include "../../graphics/graphics_layer.h" /// `graphics_layer`.


/* Predef */

struct Multi_Sprite load_multi_sprite(const char *const texture_path, const size_t max_count, int *const exit_code);
void                free_multi_sprite(struct Multi_Sprite *const target);
void add_to_multi_sprite(struct Multi_Sprite *const to, const SDL_FRect new_rects, int *const exit_code);
void render_multi_sprite(const struct Multi_Sprite *const target);


/* Body */

struct Multi_Sprite load_multi_sprite(const char *const texture_path, const size_t max_count, int *const exit_code)
{
    struct Multi_Sprite result = {0};

    /// Param checking
    if (exit_code == NULL)
        print_warning("`new_multi_sprite()`: `exit_code` arg is `NULL`");
    if (max_count == 0)
    {
        print_warning("`new_multi_sprite()`: `max_count` arg is 0. `Multi_Texture` is not dynamic-memory");
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (texture_path == NULL)
    {
        print_error("`new_multi_sprite()`: `texture_path` arg is `NULL`");
        result.texture = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Object creation
    result.texture = IMG_LoadTexture(graphics_layer.renderer, texture_path);
    if (result.texture == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_SDL_warning("`new_multi_sprite()`: couldn't load the texture, replaced with null texture");
            result.texture = NULL_TEXTURE;
        }
        else
        {
            print_SDL_error("`new_multi_sprite()`: couldn't load the texture, and null texture is empty");
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }
    result.rects = calloc(max_count, sizeof(SDL_FRect));
    if (result.rects == NULL)
    {
        print_error("`new_multi_sprite()`: couldn't allocate memory to coordinates");
        SDL_DestroyTexture(result.texture);
        result.texture = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.max_count = max_count;
    
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_multi_sprite(struct Multi_Sprite *const target)
{
    if (target == NULL)
        return;
    
    if (target->texture != NULL && target->texture != NULL_TEXTURE)
        SDL_DestroyTexture(target->texture);
    if (target->rects != NULL)
        free(target->rects);

    memset(target, 0, sizeof *target);
    return;
}


/* Functions */

void add_to_multi_sprite(struct Multi_Sprite *const to, const SDL_FRect new_rect, int *const exit_code)
{
    /// Param checking
    if (exit_code == NULL)
        print_warning("`add_to_multi_sprite()`: `exit_code` arg is `NULL`");
    if (to == NULL)
    {
        print_error("`add_to_multi_sprite()`: `to` arg is `NULL`");
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (to->rects == NULL)
    {
        print_error("`add_to_multi_sprite()`: `to->rects` is `NULL`");
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Check if full
    if (to->max_count == to->cur_count)
    {
        print_error("`add_to_multi_sprite()`: `to->rects` is full");
        return;
    }

    /// Insertion
    to->rects[to->cur_count++] = new_rect;
    return;
}


void render_multi_sprite(const struct Multi_Sprite *const target)
{
    if (target == NULL || target->texture == NULL || target->rects == NULL || target->cur_count == 0)
    {
        print_error("`render_multi_sprite()`: `target` or its members are invalid");
        return;
    }
    
    for (size_t i = 0; i < target->cur_count; i++)
    {
        if (target->rects[i].x + target->rects[i].w <= 0 ||
            target->rects[i].y + target->rects[i].h <= 0 ||
            target->rects[i].x >= RENDER_WIDTH ||
            target->rects[i].y >= RENDER_HEIGHT)
        {
            print_warning("`render_multi_sprite()`: texture rendering out of bounds");
            continue; /// While it's not an error, why waste a draw call on something not seen anyway?
        }
        else
            SDL_RenderTexture(graphics_layer.renderer, target->texture, NULL, &target->rects[i]);
    }

    return;
}