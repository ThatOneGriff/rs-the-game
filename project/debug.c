/* Related header */
#include "debug.h"

/* Headers */
#include <SDL3/SDL.h>          /// SDL_GetError().
#include <stdio.h>             /// I/O.
#include "logs.h"              /// Auto-logging if ! STDIO_ENABLED.
#include "helpers/helpers.h"   /// Coloured text.


/* Predef */

void print_success(const char *const text);
void print_error  (const char *const text);
void print_warning(const char *const text);
void print_SDL_error  (const char *const text);
void print_SDL_warning(const char *const text);


/* Body */

void print_success(const char *const text)
{
    if (text == NULL)
        return;
    #if ! STDIO_ENABLED
        log_success(text);
        return;
    
    #elif STDIO_ENABLED
    textcolor(GREEN);
    fprintf(stderr, "~ [SUCCESS] ");
    textcolor(WHITE);

    fprintf(stderr, "%s.\n", text);
    return;
    #endif /// STDIO_ENABLED
}


void print_error(const char *const text)
{
    if (text == NULL)
        return;
    #if ! STDIO_ENABLED
        log_error(text);
        return;
    
    #elif STDIO_ENABLED
    textcolor(RED);
    fprintf(stderr, "~ [ERROR] ");
    textcolor(WHITE);

    fprintf(stderr, "%s.\n", text);
    return;
    #endif /// STDIO_ENABLED
}


void print_warning(const char *const text)
{
    if (text == NULL)
        return;
    #if ! STDIO_ENABLED
        log_warning(text);
        return;
    
    #elif STDIO_ENABLED
    textcolor(YELLOW);
    fprintf(stderr, "~ [WRNNG] ");
    textcolor(WHITE);

    fprintf(stderr, "%s.\n", text);
    return;
    #endif /// STDIO_ENABLED
}


void print_SDL_error(const char *const text)
{
    if (text == NULL)
        return;
    #if ! STDIO_ENABLED
        log_SDL_error(text);
        return;
    
    #elif STDIO_ENABLED
    textcolor(RED);
    fprintf(stderr, "~ [ERROR] ");
    textcolor(WHITE);

    fprintf(stderr, "%s: \"%s\".\n", text, SDL_GetError());
    return;
    #endif /// STDIO_ENABLED
}


void print_SDL_warning(const char *const text)
{
    if (text == NULL)
        return;
    #if ! STDIO_ENABLED
        log_SDL_warning(text);
        return;
    
    #elif STDIO_ENABLED
    textcolor(YELLOW);
    fprintf(stderr, "~ [WRNNG] ");
    textcolor(WHITE);

    fprintf(stderr, "%s: \"%s\".\n", text, SDL_GetError());
    return;
    #endif /// STDIO_ENABLED
}