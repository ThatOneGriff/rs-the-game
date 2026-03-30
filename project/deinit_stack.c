/* Related header */
#include "deinit_stack.h"

/* Headers */
#include <stdlib.h> /// `*alloc()`.
#include <string.h> /// `memset()`.
#include "debug.h"  /// Error printing.


/* Predef */

struct Deinit_Stack new_deinit_stack(const size_t size, int *const exit_code);
void               free_deinit_stack(struct Deinit_Stack *const target); 
void             add_to_deinit_stack(struct Deinit_Stack *const target, void *const new_element, void (*const new_free_function)(void*));
void              flush_deinit_stack(struct Deinit_Stack *const target);
void           pop_from_deinit_stack(struct Deinit_Stack *const target);


/* Body */

struct Deinit_Stack new_deinit_stack(const size_t size, int *const exit_code)
{
    if (exit_code == NULL)
        print_warning("`new_deinit_stack()`: `exit_code` arg is `NULL`");
    struct Deinit_Stack result = {0};

    result.elements = calloc(size, sizeof(void*));
    if (result.elements == NULL)
    {
        print_error("`new_deinit_stack()`: couldn't allocate memory for elements' array");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.free_functions = calloc(size, sizeof(void (*)(int*)));
    if (result.free_functions == NULL)
    {
        print_error("`new_deinit_stack()`: couldn't allocate memory for free_functions");
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

void free_deinit_stack(struct Deinit_Stack *const target)
{
    if (target == NULL)
        return;
    
    /// NOTE: we are not freeing members,
    /// because they're pointers to to-be-used parts of program.
    if (target->elements != NULL)
        free(target->elements);
    
    if (target->free_functions != NULL)
        free(target->free_functions);

    memset(target, 0, sizeof *target);
    return;
}


/* Body */

/// NOTE: unlike other structures (`Multi/Shifting_Texture`), which have a use case to being dynamic,
/// making `Deinit_Stack` non-fixed-size could cause confusion and unneccessary memory usage
/// without good benefits. So, the size of `Deinit_Stack::elements` is set one time upon creation.
/// - Say, you wanted to add 7 elements to the stack, but during development it so happened that you started adding 8.
/// 1) Wouldn't you want to know of such a correction, instead of the program quietly multiplying the stack's memory by 1.5?
/// 2) Whose problem would that additional memory be?
void add_to_deinit_stack(struct Deinit_Stack *const target, void *const new_element, void (*const new_free_function)(void*))
{
    if (target == NULL || target->elements == NULL)
    {
        print_error("`add_to_deinit_stack()`: stack or its `elements` are `NULL`");
        return;
    }
    if (new_element == NULL)
    {
        print_error("`add_to_deinit_stack()`: `new_element` is `NULL`");
        return;
    }
    if (new_free_function == NULL)
    {
        print_error("`add_to_deinit_stack()`: `new_free_function` is `NULL`");
        return;
    }
    if (target->cur == target->max)
    {
        print_error("`add_to_deinit_stack()`: stack is full");
        return;
    }

    target->elements [target->cur] = new_element;
    target->free_functions[target->cur] = new_free_function;
    ++target->cur;
}


/// `free`'s the stack at the end, too.
void flush_deinit_stack(struct Deinit_Stack *const target)
{
    if (target == NULL || target->elements == NULL || target->free_functions == NULL)
    {
        print_error("`flush_deinit_stack()`: stack, its `elements` or its `functions` are `NULL`");
        return;
    }

    while (target->cur > 0)
        pop_from_deinit_stack(target);
    free_deinit_stack(target);
}


void pop_from_deinit_stack(struct Deinit_Stack *const target)
{
    if (target == NULL || target->elements == NULL || target->free_functions == NULL)
    {
        print_error("`pop_from_deinit_stack()`: stack, its `elements` or its `functions` are `NULL`");
        return;
    }
    if (target->cur == 0)
    {
        print_error("`pop_from_deinit_stack()`: stack is empty");
        return;
    }

    --target->cur;
    if (target->elements[target->cur] != NULL)
    {
        target->free_functions[target->cur](target->elements[target->cur]);
        ///printf("[freed] "); /// For demonstration purposes.
    }
    return;
}