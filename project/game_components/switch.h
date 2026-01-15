#pragma once
#ifndef SWITCH_H
#define SWITCH_H

#include <stdlib.h>   /// `*alloc()`.
#include <stdbool.h>  /// `bool is_focused()`.
#include "button.h"   /// Buttons.
#include "../debug.h" /// Error message printing.
#include "../graphics/graphics_layer.h" /// `graphics_layer.renderer`.


/* Struct */

struct Switch
{
    bool is_focused;
    struct Button* options;
    size_t max_option_count;
    size_t cur_option_count;
    size_t cur_option;
};


/* Predef */

struct Switch init_switch(const size_t max_option_count, int* exit_code);
void          free_switch(struct Switch* target);
void        add_to_switch(struct Switch* target, const struct Button new_option);
void change_switch_option(struct Switch* target);
void        render_switch(struct Switch* target);


/* Body */

struct Switch init_switch(const size_t max_option_count, int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`init_switch()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Object preparation
    struct Switch result;
    result.is_focused = false;
    result.options      = NULL;
    result.max_option_count = 0; /// Temporary value to be changed once memory is successfully allocated.
    result.cur_option_count = 0;
    result.cur_option = 0;

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


void free_switch(struct Switch* target)
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
        target->options = NULL;
    }
    target->max_option_count = 0;
    target->cur_option_count = 0;
    target->cur_option = 0;
    return;
}


void add_to_switch(struct Switch* target, const struct Button new_option)
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


void change_switch_option(struct Switch* target)
{
    if (target == NULL)
    {
        print_error("`change_switch_option()`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }
    
    ++target->cur_option;
    if (target->cur_option == target->cur_option_count)
        target->cur_option = 0;
    return;
}


void render_switch(struct Switch* target)
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

#endif /// SWITCH_H