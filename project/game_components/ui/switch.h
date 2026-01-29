#pragma once
#ifndef SWITCH_H
#define SWITCH_H

/* C headers */
#include <stdbool.h>  /// `bool is_focused()`.

/* Helper headers */
#include "button.h"   /// Buttons.


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

#endif /// SWITCH_H