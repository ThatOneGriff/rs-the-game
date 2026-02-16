
#ifndef DEINIT_STACK_H
#define DEINIT_STACK_H

//#include "debug.h" /// Error printing.
#include <stdlib.h> /// `*alloc()`.


/* Predef */

struct Deinit_Stack;
struct Deinit_Stack new_deinit_stack(const size_t size, int* exit_code);
void               free_deinit_stack(struct Deinit_Stack* target); 
void             add_to_deinit_stack(struct Deinit_Stack* target, void* new_element, void (*new_function)(void*));
void              flush_deinit_stack(struct Deinit_Stack* target);
void           pop_from_deinit_stack(struct Deinit_Stack* target);


/* Struct */

struct Deinit_Stack
{
    size_t max;
    size_t cur;
    void**  elements;
    void (**functions)(void*);
};

struct Deinit_Stack new_deinit_stack(const size_t size, int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`new_deinit_stack()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    struct Deinit_Stack result;
    result.cur = 0;
    result.max = 0; /// To be reset once memory is successfully allocated.
    result.functions = NULL; /// To be reset once memory is successfully allocated.

    result.elements = calloc(size, sizeof(void*));
    if (result.elements == NULL)
    {
        //print_error("`new_deinit_stack()`: couldn't allocate memory for elements' array", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    /*for (size_t i = 0; i < size; i++)
    {
        result.elements[i] = malloc(sizeof(void*));

        if (result.elements[i] == NULL)
        {
            //print_error("`new_deinit_stack()`: couldn't allocate memory for an element", NON_SDL_ERROR);
            for (size_t j = 0; j < i; j++)
            {
                free(result.elements[j]);
                result.elements[j] = NULL;
            }
            free(result.elements);
            result.elements = NULL;
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }*/

    result.functions = calloc(size, sizeof(void (*)(int*)));
    if (result.functions == NULL)
    {
        //print_error("`new_deinit_stack()`: couldn't allocate memory for functions", NON_SDL_ERROR);
        for (size_t i = 0; i < size; i++)
        {
            free(result.elements[i]);
            result.elements[i] = NULL;
        }
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
        for (size_t i = 0; i < target->max; i++)
        {
            free(target->elements[i]);
            target->elements[i] = NULL;
        }
        free(target->elements);
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
void add_to_deinit_stack(struct Deinit_Stack* target, void* new_element, void (*new_function)(void*))
{
    if (target == NULL || target->elements == NULL)
    {
        //print_error("`add_to_deinit_stack()`: stack or its `elements` are `NULL`", NON_SDL_ERROR);
        return;
    }
    if (new_element == NULL || new_function == NULL)
    {
        //print_error("`add_to_deinit_stack()`: `new_element` or `new_function` are `NULL`", NON_SDL_ERROR);
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


void flush_deinit_stack(struct Deinit_Stack* target)
{
    if (target == NULL || target->elements == NULL || target->functions == NULL)
    {
        //print_error("`pop_from_deinit_stack()`: stack, its `elements` or its `functions` are `NULL`", NON_SDL_ERROR);
        return;
    }

    printf("%llu %llu\n", target->cur, target->max);
    while (target->cur > 0)
        pop_from_deinit_stack(target);
}


void pop_from_deinit_stack(struct Deinit_Stack* target)
{
    if (target == NULL || target->elements == NULL || target->functions == NULL)
    {
        //print_error("`pop_from_deinit_stack()`: stack, its `elements` or its `functions` are `NULL`", NON_SDL_ERROR);
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