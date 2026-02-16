
#ifndef DEINIT_STACK_H
#define DEINIT_STACK_H

#include <stdlib.h> /// `size_t`.

/* Struct */

struct Deinit_Stack
{
    size_t max;
    size_t cur;
    void**  elements;
    void (**free_functions)(void*);
};


/* Predef */

struct Deinit_Stack new_deinit_stack(const size_t size, int *const exit_code);
void               free_deinit_stack(struct Deinit_Stack *const target); 
void             add_to_deinit_stack(struct Deinit_Stack *const target, void *const new_element, void (*const new_free_function)(void*));
void              flush_deinit_stack(struct Deinit_Stack *const target);
void           pop_from_deinit_stack(struct Deinit_Stack *const target);


#endif /// DEINIT_STACK_H