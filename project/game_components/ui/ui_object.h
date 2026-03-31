#ifndef UI_OBJECT_H
#define UI_OBJECT_H
#include <stdbool.h> /// `bool focus`.
#include <stddef.h>  /// `offsetof()`.


/* Defines */

/// This is to identify neighbors.
/// Not an `enum` 'cause I don't want to write `button.neighbors[Neighbors::Up]`.
#define UI_NEIGHBORS_COUNT 4
#define UP    0
#define DOWN  1
#define LEFT  2
#define RIGHT 3


/* Predef */

void  set_focus   (void *const ui_obj, const bool focus);
void* get_neighbor(void *const ui_obj, const unsigned short int direction);
void  set_neighbor(void *const ui_obj, const unsigned short int direction, void *const new_neighbor);

#endif /// UI_OBJECT_H