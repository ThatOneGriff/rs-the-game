/* Related header */
#include "helpers.h"

/* Headers */
#include <stdio.h> /// `size_t`.
#include <stdlib.h>  /// `free()`.
#include <windows.h> /// Console text coloring.


/* Text coloring */

void textcolor(const short int color_code)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_code);
}


/* Memory */

void free_ptr_arr(void** target, const size_t len)
{
    if (target == NULL)
        return;
    
    for (size_t i = 0; i < len; i++)
    {
        free(target[i]);
        target[i] = NULL;
    }

    free(target);
    return;
}