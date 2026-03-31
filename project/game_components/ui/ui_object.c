/* Related headers */
#include "ui_object.h"
#include "button.h"


/* Defines */

/// This is needed for raw-memory manipulation of `struct Button` and `struct Switch` indistinctively.
/// This implies that both `struct Button` and `struct Switch` must share a common structure in the beginning.
#define UI_IS_FOCUSED_OFFSET offsetof(struct Button, is_focused)
#define  UI_NEIGHBORS_OFFSET offsetof(struct Button, neighbors)


/* Predef */

void  set_focus   (void *const ui_obj, const bool focus);
void* get_neighbor(void *const ui_obj, const unsigned short int direction);
void  set_neighbor(void *const ui_obj, const unsigned short int direction, void *const new_neighbor);


/* Body */

void set_focus(void *const ui_obj, const bool focus)
{
    bool *const obj_is_focused = (bool*)((char*)ui_obj + UI_IS_FOCUSED_OFFSET);
    *obj_is_focused = focus;
    return;
}


void* get_neighbor(void *const ui_obj, const unsigned short int direction)
{
    uintptr_t addr = (uintptr_t)ui_obj + UI_NEIGHBORS_OFFSET;
    void** neighbors = (void**)addr;
    return neighbors[direction];
}


void set_neighbor(void *const ui_obj, const unsigned short int direction, void *const new_neighbor)
{
    uintptr_t addr = (uintptr_t)ui_obj + UI_NEIGHBORS_OFFSET;
    void** neighbors = (void**)addr;
    neighbors[direction] = new_neighbor;
    return;
}