/* Related headers */
#include "ui_neighbor.h"
#include "button.h"
#include "switch.h"

/* Headers */
#include "../../debug.h" /// Error message printing.


/* Predef */

void focus_ui_neighbor(struct UI_Neighbor *const target);
void  free_ui_neighbor(struct UI_Neighbor *const target);


/* Body */

void focus_ui_neighbor(struct UI_Neighbor *const target)
{
    switch (target->type)
    {
    case (UI_Neighbor_type::undefined):
        print_error("`focus_ui_neighbor()`: can't focus a `struct UI_Neighbor` of type `undefined`");
        return;
    
    case (UI_Neighbor_type::button_t):
        (struct Button)(*target->pointer).is_focused = true;
        return;
    
    case (UI_Neighbor_type::switch_t):
        (struct Switch)(*target->pointer).is_focused = true;
        return;
    }
}


void free_ui_neighbor(struct UI_Neighbor *const target)
{
    target->pointer = NULL; /// We don't free any memory, as it points to an already existing object.
    target->type    = UI_Neighbor_type::undefined;
    return;
}