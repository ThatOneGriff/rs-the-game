#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <SDL3/SDL.h> /// SDL_GetError()
#include <stdio.h>    /// I/O
#include <stdbool.h>  /// `bool` args for `print_*`

#include "helpers/helpers.h"     /// Coloured text

#define UNUSED(expr) do { (void)(expr); } while (0)
#define SDL_ERROR     true
#define NON_SDL_ERROR false


/* Predef */

void print_error(  char* text, bool is_SDL_error);
void print_warning(char* text, bool is_SDL_error);


/* Body */

void print_error(char* text, bool is_SDL_error)
{
    if (text == NULL)
        return;
    
    textcolor(RED);
    printf("~ [ERROR] ");
    textcolor(WHITE);

    printf("%s", text);
    if (is_SDL_error)
        printf(": %s.\n", SDL_GetError());
    else
        printf(".\n");
}


void print_warning(char* text, bool is_SDL_error)
{
    if (text == NULL)
        return;
    
    textcolor(YELLOW);
    printf("~ [WRNNG] ");
    textcolor(WHITE);

    printf("%s", text);
    if (is_SDL_error)
        printf(": %s.\n", SDL_GetError());
    else
        printf(".\n");
}

#endif /// DEBUG_H