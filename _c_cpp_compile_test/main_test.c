#include <stdio.h>
#include "function.h"

/*
g++ -c function.cpp -o function.o
gcc -c main_test.c -o main_test.o
g++ function.o main_test.o -o program
*/

int main()
{
    printf("Calling a C++ function from C:\n");
    cpp_function();
    return 0;
}