#ifndef UI_NEIGHBOR_H
#define UI_NEIGHBOR_H


/* Structs */

enum UI_Neighbor_type
{
    undefined = 0, /// Is used to indicate that memory is free and can be filled immediately; ONLY for `UI_Neighbor`'s passed as `dst`.
    button_t  = 1,
    switch_t  = 2
};

struct UI_Neighbor
{
    void* pointer;
    enum UI_Neighbor_type type;
};


/* Predef */

void focus_ui_neighbor(struct UI_Neighbor *const target);
void  free_ui_neighbor(struct UI_Neighbor *const target);


#endif /// UI_NEIGHBOR_H