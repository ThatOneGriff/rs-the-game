#ifndef SWITCH_H
#define SWITCH_H

/* Headers */
#include <stdbool.h>  /// `bool is_focused()`.
#include "button.h"   /// Buttons.


/* Struct */

struct Switch
{
    bool is_focused;
    struct Button* options;
    struct Button* cur_option;

    size_t max_option_count;
    size_t cur_option_count;
    size_t cur_option_id;
};


/* Predef */

struct Switch init_switch(const size_t max_option_count, int *const exit_code);
void          free_switch(struct Switch *const target);
void        add_to_switch(struct Switch *const target, const struct Button new_option);
void   next_switch_option(struct Switch *const target);
void        render_switch(struct Switch *const target);


#endif /// SWITCH_H