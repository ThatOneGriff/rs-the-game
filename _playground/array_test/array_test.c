#include <stdio.h>
#include <stdlib.h>

/*
gcc array_test.c -o array_test.exe -Wall -Wextra -Werror -std=c23
./array_test.exe
*/

struct Array
{
    int* array;
    size_t size;
};

struct Array new_array(const int* values, const size_t size)
{
    struct Array result;
    result.size = size;
    result.array = calloc(size, sizeof(int));

    for (size_t i = 0; i < size; i++)
        result.array[i] = values[i];
    return result;
}


int main()
{
    /// TODO: test if I can do `new_array({1,2,3,4}, 4)`

    return 0;
}