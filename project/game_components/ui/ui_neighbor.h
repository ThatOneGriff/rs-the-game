#ifndef UI_NEIGHBOR_H
#define UI_NEIGHBOR_H


/* Struct predef */

struct Button;
struct Switch;


/* Struct */

union UI_Neighbor
{
    struct Button* btn_ptr;
    struct Switch*  sw_ptr;
};


/* Predef */

void couple(union UI_Neighbor dst, union UI_Neighbor src);


#endif /// UI_NEIGHBOR_H