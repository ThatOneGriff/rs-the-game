#include <stdio.h> /// I/O.
#include "deinit_stack.h"
#include "test_functions.h"

/*
gcc test.c -o test.exe -Wall -Werror -Wextra -std=c23
./test.exe
*/


int main()
{
    int exit_code = EXIT_SUCCESS;

    struct Deinit_Stack test_stack = new_deinit_stack(5, &exit_code);
    if (exit_code == EXIT_FAILURE)
    {
        //error message print if needed
        return EXIT_FAILURE;
    }

    printf("Enter int, char and float:\n> ");
    int inp_i = (getchar() - '0');
    add_to_deinit_stack(&test_stack, &inp_i, (void (*)(void*))p_int);

    char inp_c = getchar();
    add_to_deinit_stack(&test_stack, &inp_c, (void (*)(void*))p_chr);

    float inp_f;
    scanf("%f", &inp_f);
    add_to_deinit_stack(&test_stack, &inp_f, (void (*)(void*))p_flt);

    for (size_t i = 0; i < 3; i++)
        pop_from_deinit_stack(&test_stack);

    return exit_code;
}