#pragma once
#ifndef DEBUG_H
#define DEBUG_H

/* Headers */
#include <SDL3/SDL.h> /// SDL_GetError().
#include <stdio.h>    /// I/O.
#include <stdbool.h>  /// `bool` args for `print_*()`.

#include "helpers/helpers.h" /// Coloured text.

#define UNUSED(expr) do { (void)(expr); } while (0)
#define  IS_SDL_ERROR true
#define NON_SDL_ERROR false

//#define USING_AUDIO /// Compilation with audio is VERY long. Comment if not needed.


/* Predef */

void print_success(char* text);
void print_error(  char* text, bool is_SDL_error);
void print_warning(char* text, bool is_SDL_error);


/* Body */

void print_success(char* text)
{
    if (text == NULL)
        return;
    
    textcolor(GREEN);
    fprintf(stderr, "~ [SUCCESS] ");
    textcolor(WHITE);

    fprintf(stderr, "%s.\n", text);
}


void print_error(char* text, bool is_SDL_error)
{
    if (text == NULL)
        return;
    
    textcolor(RED);
    fprintf(stderr, "~ [ERROR] ");
    textcolor(WHITE);

    fprintf(stderr, "%s", text);
    if (is_SDL_error)
        fprintf(stderr, ": \"%s\".\n", SDL_GetError());
    else
        fprintf(stderr, ".\n");
}


void print_warning(char* text, bool is_SDL_error)
{
    if (text == NULL)
        return;
    
    textcolor(YELLOW);
    fprintf(stderr, "~ [WRNNG] ");
    textcolor(WHITE);

    fprintf(stderr, "%s", text);
    if (is_SDL_error)
        fprintf(stderr, ": \"%s\".\n", SDL_GetError());
    else
        fprintf(stderr, ".\n");
}

#endif /// DEBUG_H