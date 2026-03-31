#ifndef UI_NEIGHBOR_H
#define UI_NEIGHBOR_H


/* Structs' Predef */

struct Button;
struct Switch;


/* Structs */

union UI_Neighbor_ptr
{
    struct Button* button_p;
    struct Switch* switch_p;
};

enum UI_Neighbor_type
{
    undefined = 0, /// Is used to indicate that memory is free and can be filled immediately; ONLY for `UI_Neighbor`'s passed as `dst`.
    button_t  = 1,
    switch_t  = 2
};

struct UI_Neighbor
{
    union UI_Neighbor_ptr  pointer;
    enum  UI_Neighbor_type type;
};


/* Predef */

void couple(struct UI_Neighbor *const dst, struct UI_Neighbor *const src);
void focus (struct UI_Neighbor *const target);
void free  (struct UI_Neighbor *const target);


#endif /// UI_NEIGHBOR_H