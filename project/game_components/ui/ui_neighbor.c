/* Related headers */
#include "ui_neighbor.h"
#include "button.h"
#include "switch.h"

/* Headers */
#include "../../debug.h" /// Error message printing.


/* Predef */

void couple(struct UI_Neighbor *const dst, struct UI_Neighbor *const src);
void focus (struct UI_Neighbor *const target);
void free  (struct UI_Neighbor *const target);
static void couple_button(struct UI_Neighbor *const dst, struct UI_Neighbor *const src);
static void couple_switch(struct UI_Neighbor *const dst, struct UI_Neighbor *const src);


/* Body */

void couple(struct UI_Neighbor *const dst, struct UI_Neighbor *const src)
{
    switch (src->type)
    {
    case (UI_Neighbor_type::button_t):
        couple_button(dst, src);
        return;
    
    case (UI_Neighbor_type::switch_t)
        couple_switch(dst, src);
        return;
    
    case (UI_Neighbor_type::undefined)
        print_error("`couple()`: `src->type` cannot be undefined");
        return;
    }

    return;
}


void free(struct UI_Neighbor *const target)
{
    target->pointer = NULL; /// We don't free any memory, as it merely points to an already existing object.
    target->type    = UI_Neighbor_type::undefined;
    return;
}


static void couple_button(struct UI_Neighbor *const dst, struct UI_Neighbor *const src)
{
    /// We may ignore check for `src->type`: it's guaranteed to only be called through `couple()`, which already does the check.

    if (dst->type != UI_Neighbor_type::undefined)
        free(dst);
    
    dst->pointer::bu
    return;
}