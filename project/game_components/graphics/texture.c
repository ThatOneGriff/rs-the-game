/* Related header */
#include "texture.h"

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


/// NOTE: What I call `Texture` is, in fact, a sprite.
/// I have been told that too deep into development.


/* Predef */

struct Texture load_texture(const char *const path, const SDL_FRect rect, int *const exit_code);
void           free_texture(struct Texture *const target);
void   render_texture(const struct Texture *const target);


/* Body */

struct Texture load_texture(const char *const path, const SDL_FRect rect, int *const exit_code)
{
    struct Texture result = {0};

    /// Parameter checking
    if (exit_code == NULL)
        print_warning("`load_texture()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (rect.w == 0.0f || rect.h == 0.0f)
        print_warning("`load_texture()`: `rect`'s `x` or `y` == 0.0. Are you sure?", NON_SDL_ERROR);
    if (path == NULL)
    {
        print_error("`load_texture()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    
    /// Texture loading
    result.texture = IMG_LoadTexture(graphics_layer.renderer, path);
    if (result.texture == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`load_texture()`: couldn't load the texture, replaced with null texture", IS_SDL_ERROR);
            result.texture = NULL_TEXTURE;
        }
        else
        {
            print_error("`load_texture()`: couldn't load the texture, and null texture is empty", IS_SDL_ERROR);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }

    result.rect = rect;
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_texture(struct Texture *const target)
{
    if (target == NULL)
        return;

    if (target->texture != NULL && target->texture != NULL_TEXTURE)
        SDL_DestroyTexture(target->texture);
    
    memset(target, 0, sizeof *target);
    return;
}


void render_texture(const struct Texture *const target)
{
    /* Param checking */
    if (target == NULL || target->texture == NULL)
    {
        print_error("`render_texture()` target or its `texture` is `NULL`", NON_SDL_ERROR);
        return;
    }
    if (target->rect.w == 0.0f || target->rect.h == 0.0f)
        print_warning("`render_texture()`: target's `w` or `h` == 0.0. Are you sure this was supposed to happen?", NON_SDL_ERROR);
    if (target->rect.x + target->rect.w <= 0.0f ||
        target->rect.y + target->rect.h <= 0.0f ||
        target->rect.x >= RENDER_WIDTH ||
        target->rect.y >= RENDER_HEIGHT)
    {
        print_warning("`render_texture()`: texture rendering out of bounds", NON_SDL_ERROR);
        return; /// While it's not an error, why waste a draw call on something not seen anyway?
    }
    
    /* Rendering */
    SDL_RenderTexture(graphics_layer.renderer, target->texture, NULL, &target->rect);
    return;
}