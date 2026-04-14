/* Related header */
#include "switch.h"

/* C headers */
#include <stdlib.h>  /// `*alloc()`.
#include <stdbool.h> /// `bool is_focused()`.
#include <string.h>  /// `memset()`.

/* Helper headers */
#include "pseudo_button.h"                 /// Switches' options.
#include "../../debug.h"                   /// Error message printing.
#include "../../graphics/graphics_layer.h" /// `graphics_layer.renderer`.


/* Predef */

struct Switch    init_switch(const size_t max_option_count, int *const exit_code);
void add_neighbors_to_switch(struct Switch *const target, void *const up, void *const down, void *const left, void *const right);
void             free_switch(struct Switch *const target);
void           add_to_switch(struct Switch *const target, const struct Pseudo_Button new_option);
void      next_switch_option(struct Switch *const target);
void           render_switch(struct Switch *const target);


/* Body */

struct Switch init_switch(const size_t max_option_count, int *const exit_code)
{
    if (exit_code == NULL)
        print_warning("`init_switch()`: `exit_code` arg is `NULL`");
    
    /// Object preparation
    struct Switch result = {0};

    if (max_option_count == 0)
    {
        print_error("`init_switch()`: `max_option_count` arg must be > 0");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Memory allocation
    result.options = malloc(max_option_count * sizeof(struct Pseudo_Button));
    if (result.options == NULL)
    {
        print_error("`init_switch()`: couldn't allocate memory");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.max_option_count = max_option_count;
    result.cur_option = &result.options[0];
    *exit_code = EXIT_SUCCESS;
    return result;
}


void add_neighbors_to_switch(struct Switch *const target, void *const up, void *const down, void *const left, void *const right)
{
    if (target == NULL)
    {
        print_error("`add_neighbors_to_switch()`: `target` arg is `NULL`");
        return;
    }

    target->neighbors[UP]    = up;
    target->neighbors[DOWN]  = down;
    target->neighbors[LEFT]  = left;
    target->neighbors[RIGHT] = right;
    return;
}


void free_switch(struct Switch *const target)
{
    if (target == NULL)
    {
        print_error("`free_switch()`: `target` arg is `NULL`");
        return;
    }

    if (target->options != NULL)
    {
        for (size_t i = 0; i < target->cur_option_count; i++)
            free_pseudo_button(&target->options[i]);
        free(target->options);
    }
    
    memset(target, 0, sizeof *target);
    return;
}


void add_to_switch(struct Switch *const target, const struct Pseudo_Button new_option)
{
    if (target == NULL || target->options == NULL)
    {
        print_error("`add_to_switch()`: `target` arg or its memory is `NULL`");
        return;
    }
    if (target->cur_option_count == target->max_option_count)
    {
        print_error("`add_to_switch()`: `target` memory is full");
        return;
    }

    target->options[target->cur_option_count++] = new_option;
    return;
}


void next_switch_option(struct Switch *const target)
{
    if (target == NULL)
    {
        print_error("`next_switch_option()`: `target` arg is `NULL`");
        return;
    }
    
    ++target->cur_option_id;
    if (target->cur_option_id == target->cur_option_count)
        target->cur_option_id = 0;
    target->cur_option = &target->options[target->cur_option_id];
    return;
}


void render_switch(struct Switch *const target)
{
    if (target == NULL) /// TODO: check all members.
    {
        print_error("`render_switch()`: `target` arg is `NULL`");
        return;
    }

    if (target->is_focused)
        render_sprite(&target->cur_option->focused_texture);
    else
        render_sprite(&target->cur_option->regular_texture);
    return;
}