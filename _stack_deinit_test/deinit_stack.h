#pragma once
#ifndef DEINIT_STACK_H
#define DEINIT_STACK_H

//#include "debug.h" /// Error printing.
#include <stdlib.h> /// `*alloc()`.

/// TEMP: `void*` replaced with `int` for elements.

/* Predef */

struct Deinit_Stack;
struct Deinit_Stack new_deinit_stack(const size_t size, int* exit_code);
void               free_deinit_stack(struct Deinit_Stack* target); 
void             add_to_deinit_stack(struct Deinit_Stack* target, const int new_element);
int            pop_from_deinit_stack(struct Deinit_Stack* target);


/* Struct */

struct Deinit_Stack
{
    size_t max_elements;
    size_t cur_elements;
    int* elements;
};

struct Deinit_Stack new_deinit_stack(const size_t size, int* exit_code)
{
    if (exit_code == NULL)
        {;} /// TEMP
    //    print_warning("`new_deinit_stack()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    struct Deinit_Stack result;
    result.cur_elements = 0;
    result.max_elements = 0; /// To be reset once memory is successfully allocated.

    result.elements = calloc(size, sizeof(int));
    if (result.elements == NULL)
    {
        //print_error("`new_deinit_stack()`: couldn't allocate memory", NON_SDL_ERROR);
        //*exit_code = EXIT_FAILURE;
        return result;
    }
    result.max_elements = size;

    *exit_code = EXIT_SUCCESS;
    return result;
}

void free_deinit_stack(struct Deinit_Stack* target)
{
    if (target == NULL)
        return;
    
    if (target->elements != NULL)
    {
        free(target->elements); /// NOTE: take double pointers into account if you happen to use them in the future.
        target->elements = NULL;
    }
    target->max_elements = 0;
    target->cur_elements = 0;
    return;
}


/* Body */

/// NOTE: unlike other structures (`Multi/Shifting_Texture`), which have a use case to being dynamic,
/// making `Deinit_Stack` non-fixed-size could cause confusion and unneccessary memory usage
/// without good benefits. So, the size of `Deinit_Stack::elements` is set once, upon creation.
void add_to_deinit_stack(struct Deinit_Stack* target, const int new_element)
{
    if (target == NULL || target->elements == NULL)
    {
        //print_error("`add_to_deinit_stack()`: stack or its `elements` are `NULL`", NON_SDL_ERROR);
        return;
    }
    if (target->cur_elements == target->max_elements)
    {
        //print_error("`add_to_deinit_stack()`: stack is full", NON_SDL_ERROR);
        return;
    }

    target->elements[target->cur_elements++] = new_element;
}


int pop_from_deinit_stack(struct Deinit_Stack* target)
{
    if (target == NULL || target->elements == NULL)
    {
        //print_error("`pop_from_deinit_stack()`: stack or its `elements` are `NULL`", NON_SDL_ERROR);
        return 0;
    }
    if (target->cur_elements == 0)
    {
        //print_error("`pop_from_deinit_stack()`: stack is empty", NON_SDL_ERROR);
        return 0;
    }

    return target->elements[--target->cur_elements];
}

#endif /// DEINIT_STACK_H