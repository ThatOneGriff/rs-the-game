/* Related header */
#include "button.h"

/* C headers */
#include <stdbool.h> /// `bool is_focused`.
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

struct Button  create_button(const char *const text, const SDL_Color inner_color, const struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int *const exit_code);
void add_neighbors_to_button(      struct Button *const target, struct Button *const up, struct Button *const down, struct Button *const left, struct Button *const right);
void           render_button(const struct Button *const target);
void             free_button(      struct Button *const target);


/* Body */

struct Button create_button(const char *const text, const SDL_Color inner_color, struct Vec2 screen_pos, const unsigned int size, const unsigned int border_thickness, int *const exit_code)
{
    struct Button result = {0};

    /* Param checking */
    if (size == 0)
    {
        print_error("`create_button()`: `size` is 0");
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (exit_code == NULL)
        print_warning("`create_button()`: `exit_code` arg is `NULL`");
    
    /* regular_texture texture */
    result.regular_texture = create_text(text, inner_color, (SDL_Color){188,204,220,255}, screen_pos, size, border_thickness/2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`create_button()`: `regular_texture` creation failure");
        return result;
    }

    /* focused_texture texture */
    const struct Vec2 border_adjusted_screen_pos = vec2(screen_pos.x - (float)border_thickness/2, screen_pos.y - (float)border_thickness/2); /// So that focusing doesn't make the button jitter.
    result.focused_texture = create_text(text, inner_color, (SDL_Color){255,255,255,255}, border_adjusted_screen_pos, size, border_thickness, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`create_button()`: `focused_texture` creation failure");
        free_sprite(&result.regular_texture);
        return result;
    }

    *exit_code = EXIT_SUCCESS;
    return result;
}


void add_neighbors_to_button(struct Button *const target, struct Button *const up, struct Button *const down, struct Button *const left, struct Button *const right)
{
    if (target == NULL)
    {
        print_error("`add_neighbors_to_button()`: `target` arg is `NULL`");
        return;
    }

    target->up    = up;
    target->down  = down;
    target->left  = left;
    target->right = right;
    return;
}


void render_button(const struct Button *const target)
{
    /* Param checking */
    if (target == NULL)
    {
        print_error("`render_button()` target is `NULL`");
        return;
    }
    
    /* Rendering */
    if (target->is_focused)
        render_sprite(&target->focused_texture);
    else
        render_sprite(&target->regular_texture);
    return;
}


void free_button(struct Button *const target)
{
    free_sprite(&target->regular_texture);
    free_sprite(&target->focused_texture);
    memset(target, 0, sizeof *target);
    return;
}