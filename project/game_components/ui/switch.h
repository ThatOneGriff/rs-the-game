#ifndef SWITCH_H
#define SWITCH_H

/* Headers */
#include <stdbool.h>       /// `bool is_focused()`.
#include "pseudo_button.h" /// Switches' options.
#include "ui_common.h"     /// Common framework for `struct Switch` and `struct Button`.


/* Struct */

struct Switch
{
    /// [!!!] UNTOUCHABLE [!!!]
    bool is_focused;

    void* neighbors[UI_NEIGHBORS_COUNT];
    /// [!!!] UNTOUCHABLE [!!!]

    struct Pseudo_Button* options;
    struct Pseudo_Button* cur_option;

    size_t max_option_count;
    size_t cur_option_count;
    size_t cur_option_id;
};


/* Predef */

struct Switch    init_switch(const size_t max_option_count, int *const exit_code);
void add_neighbors_to_switch(struct Switch *const target, void *const up, void *const down, void *const left, void *const right);
void             free_switch(struct Switch *const target);
void           add_to_switch(struct Switch *const target, const struct Pseudo_Button new_option);
void      next_switch_option(struct Switch *const target);
void           render_switch(struct Switch *const target);


#endif /// SWITCH_H