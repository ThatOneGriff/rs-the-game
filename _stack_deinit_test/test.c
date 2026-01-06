#include <stdio.h>
#include <stdlib.h>
#include "deinit_stack.h"

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

    char inp = '\0';
    for (int i = 0; i < 5; i++)
    {
        inp = getchar();
        add_to_deinit_stack(&test_stack, (int)(inp - '0'));
    }

    for (int i = 0; i < 5; i++)
    {
        printf("%d ", pop_from_deinit_stack(&test_stack));
    }

    return exit_code;
}