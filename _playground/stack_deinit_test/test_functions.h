#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

#include <stdio.h> /// I/O.


/* Predef */

void p_int(int*   n);
void p_chr(char*  c);
void p_flt(float* f);


/* Body */

void p_int(int* n)
{
    printf("p_int: %d\n", *n);
    return;
}


void p_chr(char* c)
{
    printf("p_chr: %c\n", *c);
    return;
}


void p_flt(float* f)
{
    printf("p_flt: %f\n", *f);
    return;
}

#endif /// TEST_FUNCTIONS_H