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
void             add_to_deinit_stack(struct Deinit_Stack* target, const int new_element, void (*new_function)(int));
void           pop_from_deinit_stack(struct Deinit_Stack* target);


/* Struct */

struct Deinit_Stack
{
    size_t max;
    size_t cur;
    int*    elements;
    void (**functions)(int);
};

struct Deinit_Stack new_deinit_stack(const size_t size, int* exit_code)
{
    if (exit_code == NULL)
        {;} /// TEMP
    //    print_warning("`new_deinit_stack()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    struct Deinit_Stack result;
    result.cur = 0;
    result.max = 0; /// To be reset once memory is successfully allocated.
    result.functions = NULL; /// To be reset once memory is successfully allocated.

    result.elements = calloc(size, sizeof(int));
    if (result.elements == NULL)
    {
        //print_error("`new_deinit_stack()`: couldn't allocate memory for elements", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.functions = calloc(size, sizeof(void (*)(int*)));
    if (result.functions == NULL)
    {
        //print_error("`new_deinit_stack()`: couldn't allocate memory for functions", NON_SDL_ERROR);
        free(result.elements);
        result.elements = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.max = size;
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
    if (target->functions != NULL)
    {
        free(target->functions);
        target->functions = NULL;
    }
    target->max = 0;
    target->cur = 0;
    return;
}


/* Body */

/// NOTE: unlike other structures (`Multi/Shifting_Texture`), which have a use case to being dynamic,
/// making `Deinit_Stack` non-fixed-size could cause confusion and unneccessary memory usage
/// without good benefits. So, the size of `Deinit_Stack::elements` is set one time upon creation.
/// - Say, you wanted to add 7 elements to the stack, but during development it so happened that you started adding 8.
/// 1) Wouldn't you want to know of such a correction, instead of the program quietly multiplying the stack's memory by 1.5?
/// 2) Whose problem would that additional memory be?
void add_to_deinit_stack(struct Deinit_Stack* target, const int new_element, void (*new_function)(int))
{
    if (target == NULL || target->elements == NULL)
    {
        //print_error("`add_to_deinit_stack()`: stack or its `elements` are `NULL`", NON_SDL_ERROR);
        return;
    }
    if (target->cur == target->max)
    {
        //print_error("`add_to_deinit_stack()`: stack is full", NON_SDL_ERROR);
        return;
    }

    target->elements [target->cur] = new_element;
    target->functions[target->cur] = new_function;
    ++target->cur;
}


void pop_from_deinit_stack(struct Deinit_Stack* target)
{
    if (target == NULL || target->elements == NULL)
    {
        //print_error("`pop_from_deinit_stack()`: stack or its `elements` are `NULL`", NON_SDL_ERROR);
        return;
    }
    if (target->cur == 0)
    {
        //print_error("`pop_from_deinit_stack()`: stack is empty", NON_SDL_ERROR);
        return;
    }

    --target->cur;
    target->functions[target->cur](target->elements[target->cur]);
    return;
}

#endif /// DEINIT_STACK_H