/* Headers */
#include <stdio.h>
#include <stdlib.h>

/*
gcc main.c -o ./main.exe -Wall -Wextra -Werror -std=c23
./main.exe
*/


struct Test
{
    int   i;
    char  c;
    float f;
};


int main()
{
    struct Test test_struct = {1, 'a', 3.14};
    char        test_text[] = "abc";
    
    FILE *const bin_file = fopen("binary.bin", "wb");
    fwrite(&test_struct, sizeof(test_struct), 1,                 bin_file);
    fwrite(test_text,    sizeof(char),        sizeof(test_text), bin_file);

    fclose(bin_file);
    return 0;
}