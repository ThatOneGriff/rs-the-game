#pragma once
#ifndef DEINIT_STACK_H
#define DEINIT_STACK_H


/* Predef */

struct Deinit_Stack;
struct Deinit_Stack new_deinit_stack(const size_t size);


/* Struct */

struct Deinit_Stack
{
    size_t max_elements;
    size_t cur_elements;
    int* elements;
}

void add(struct Deinit_Stack* target)
{

}

#endif /// DEINIT_STACK_H