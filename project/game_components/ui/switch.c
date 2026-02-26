/* Related header */
#include "switch.h"

/* C headers */
#include <stdlib.h>  /// `*alloc()`.
#include <stdbool.h> /// `bool is_focused()`.
#include <string.h>  /// `memset()`.

/* Helper headers */
#include "button.h"   /// Buttons.
#include "../../debug.h" /// Error message printing.
#include "../../graphics/graphics_layer.h" /// `graphics_layer.renderer`.


/* Predef */

struct Switch init_switch(const size_t max_option_count, int *const exit_code);
void          free_switch(struct Switch *const target);
void        add_to_switch(struct Switch *const target, const struct Button new_option);
void next_switch_option(struct Switch *const target);
void        render_switch(struct Switch *const target);


/* Body */

struct Switch init_switch(const size_t max_option_count, int *const exit_code)
{
    if (exit_code == NULL)
        print_warning("`init_switch()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Object preparation
    struct Switch result = {0};

    if (max_option_count == 0)
    {
        print_error("`init_switch()`: `max_option_count` arg must be > 0", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Memory allocation
    result.options = malloc(max_option_count * sizeof(struct Button));
    if (result.options == NULL)
    {
        print_error("`init_switch()`: couldn't allocate memory", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.max_option_count = max_option_count;
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_switch(struct Switch *const target)
{
    if (target == NULL)
    {
        print_error("`free_switch()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    if (target->options != NULL)
    {
        for (size_t i = 0; i < target->cur_option_count; i++)
            free_button(&target->options[i]);
        free(target->options);
    }
    
    memset(target, 0, sizeof *target);
    return;
}


void add_to_switch(struct Switch *const target, const struct Button new_option)
{
    if (target == NULL || target->options == NULL)
    {
        print_error("`add_to_switch()`: `target` arg or its memory is `NULL`", NON_SDL_ERROR);
        return;
    }
    if (target->cur_option_count == target->max_option_count)
    {
        print_error("`add_to_switch()`: `target` memory is full", NON_SDL_ERROR);
        return;
    }

    target->options[target->cur_option_count++] = new_option;
    return;
}


void next_switch_option(struct Switch *const target)
{
    if (target == NULL)
    {
        print_error("`next_switch_option()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }
    
    ++target->cur_option;
    if (target->cur_option == target->cur_option_count)
        target->cur_option = 0;
    return;
}


void render_switch(struct Switch *const target)
{
    if (target == NULL) /// TODO: check all members.
    {
        print_error("`render_switch()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    target->options[target->cur_option].is_focused = target->is_focused;
    render_button (&target->options[target->cur_option]);
    return;
}