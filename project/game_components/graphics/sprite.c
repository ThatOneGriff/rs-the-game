/* Related header */
#include "sprite.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* C headers */
#include <stdlib.h>               /// Exit codes.
#include <string.h>               /// `memset()`.

/* Helper headers */
#include "../../debug.h"                   /// Error printing.
#include "../../resources.h"               /// Null texture.
#include "../../graphics/graphics_layer.h" /// Rendering of the texture.


/* Predef */

struct Sprite load_sprite(const char *const path, const SDL_FRect rect, int *const exit_code);
void          free_sprite(struct Sprite *const target);
void        render_sprite(const struct Sprite *const target);


/* Body */

struct Sprite load_sprite(const char *const path, const SDL_FRect rect, int *const exit_code)
{
    struct Sprite result = {0};

    /// Parameter checking
    if (exit_code == NULL)
        print_warning("`load_sprite()`: `exit_code` arg is `NULL`");
    if (rect.w == 0.0f || rect.h == 0.0f)
        print_warning("`load_sprite()`: `rect`'s `x` or `y` == 0.0. Are you sure?");
    if (path == NULL)
    {
        print_error("`load_sprite()`: `path` arg is `NULL`");
        *exit_code = EXIT_FAILURE;
        return result;
    }
    
    /// Texture loading
    result.texture = IMG_LoadTexture(graphics_layer.renderer, path);
    if (result.texture == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_SDL_warning("`load_sprite()`: couldn't load the texture, replaced with null texture");
            result.texture = NULL_TEXTURE;
        }
        else
        {
            print_SDL_error("`load_sprite()`: couldn't load the texture, and null texture is empty");
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }

    result.rect = rect;
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_sprite(struct Sprite *const target)
{
    if (target == NULL)
        return;

    if (target->texture != NULL && target->texture != NULL_TEXTURE)
        SDL_DestroyTexture(target->texture);
    
    memset(target, 0, sizeof *target);
    return;
}


void render_sprite(const struct Sprite *const target)
{
    /* Param checking */
    if (target == NULL || target->texture == NULL)
    {
        print_error("`render_sprite()` target or its `texture` is `NULL`");
        return;
    }
    if (target->rect.w == 0.0f || target->rect.h == 0.0f)
        print_warning("`render_sprite()`: target's `w` or `h` == 0.0. Are you sure this was supposed to happen?");
    if (target->rect.x + target->rect.w <= 0.0f ||
        target->rect.y + target->rect.h <= 0.0f ||
        target->rect.x >= RENDER_WIDTH ||
        target->rect.y >= RENDER_HEIGHT)
    {
        print_warning("`render_sprite()`: texture rendering out of bounds");
        return; /// While it's not an error, why waste a draw call on something not seen anyway?
    }
    
    /* Rendering */
    SDL_RenderTexture(graphics_layer.renderer, target->texture, NULL, &target->rect);
    return;
}