/* Related header */
#include "pseudo_button.h"

/* C headers */
#include <stdlib.h>  /// Exit codes.
#include <string.h>  /// `memset()`.

/* SDL3 */
#include <SDL3/SDL.h> /// SDL3.

/* Helpers */
#include "../../debug.h"            /// Error printing.
#include "../../helpers/geometry.h" /// `struct Vec2`.

/* Components */
#include "../graphics/sprite.h" /// Rendering text into a texture.
#include "../text/text.h"        /// Text.


/* Predef */

struct Pseudo_Button create_pseudo_button(const char *const text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned short text_size, const unsigned short border_thickness, int *const exit_code);
void                   free_pseudo_button(struct Pseudo_Button *const target);


/* Body */

struct Pseudo_Button create_pseudo_button(const char *const text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned short text_size, const unsigned short border_thickness, int *const exit_code)
{
    struct Pseudo_Button result = {0};

    /* Param checking */
    if (exit_code == NULL)
        print_warning("`create_pseudo_button()`: `exit_code` arg is `NULL`");
    if (text_size == 0)
    {
        print_error("`create_pseudo_button()`: `text_size` is 0");
        *exit_code = EXIT_FAILURE;
        return result;
    }
    
    /* regular_texture */
    result.regular_texture = create_text(text, inner_color, (SDL_Color){188,204,220,255}, screen_pos, text_size, border_thickness/2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`create_pseudo_button()`: `regular_texture` creation failure");
        return result;
    }

    /* focused_texture */
    const struct Vec2 border_adjusted_screen_pos = vec2(screen_pos.x - (float)border_thickness / 2.0f, screen_pos.y - (float)border_thickness / 2.0f); /// So that focusing doesn't make the button's position jitter.
    result.focused_texture = create_text(text, inner_color, (SDL_Color){255, 255, 255, 255}, border_adjusted_screen_pos, text_size, border_thickness, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`create_pseudo_button()`: `focused_texture` creation failure");
        free_sprite(&result.regular_texture);
        return result;
    }

    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_pseudo_button(struct Pseudo_Button *const target)
{
    free_sprite(&target->regular_texture);
    free_sprite(&target->focused_texture);
    memset(target, 0, sizeof *target);
    return;
}