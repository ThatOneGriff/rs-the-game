#pragma once
#ifndef HELPERS_H
#define HELPERS_H

/* Headers */
#include <stdlib.h>  /// `free()`.
#include <windows.h> /// Console text coloring.


/* Text coloring. */

#define WHITE  7
#define GRAY   8
#define RED    4
#define GREEN  2
#define YELLOW 6

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
}

#endif /// HELPERS_H