/* Related header */
#include "debug.h"

/* Headers */
#include <SDL3/SDL.h> /// SDL_GetError().
#include <stdio.h>    /// I/O.
#include <stdbool.h>  /// `bool` args for `print_*()`.
#include "helpers/helpers.h" /// Coloured text.


/* Predef */

void print_success(const char* text);
void print_error  (const char* text, const bool is_SDL_error);
void print_warning(const char* text, const bool is_SDL_error);


/* Body */

void print_success(const char* text)
{
    if (text == NULL)
        return;
    
    textcolor(GREEN);
    fprintf(stderr, "~ [SUCCESS] ");
    textcolor(WHITE);

    fprintf(stderr, "%s.\n", text);
}


void print_error(const char* text, const bool is_SDL_error)
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


void print_warning(const char* text, const bool is_SDL_error)
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