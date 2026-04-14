#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h> /// `unsigned short`.


/* Text coloring */

#define WHITE  7
#define GRAY   8
#define RED    4
#define GREEN  2
#define YELLOW 6


/* Predef */

void   free_ptr_arr(void **const target, const unsigned short len);
char** read_file_by_line(const char *const path, const unsigned short target_lines);
void   textcolor(const unsigned short int color_code);


#endif /// HELPERS_H