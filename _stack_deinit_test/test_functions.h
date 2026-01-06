#pragma once
#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

#include <stdio.h> /// I/O.


/* Predef */

void plus5(int n1);
void mins5(int n2);
void mult5(int n3);
void divd5(int n4);
void equal(int n5);


/* Body */

void plus5(int n1)
{
    printf("%d + 5 = %d\n", n1, (n1+5));
    return;
}


void mins5(int n2)
{
    printf("%d - 5 = %d\n", n2, (n2-5));
    return;
}


void mult5(int n3)
{
    printf("%d * 5 = %d\n", n3, (n3*5));
    return;
}


void divd5(int n4)
{
    printf("%d / 5 = %d\n", n4, (n4/5));
    return;
}


void equal(int n5)
{
    printf("%d\n", n5);
    return;
}

#endif /// TEST_FUNCTIONS_H