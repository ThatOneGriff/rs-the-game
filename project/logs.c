/* Related header */
#include "logs.h"

/* Headers */
#include <stdio.h>             /// `fprintf()`.
#include <stdlib.h>            /// `FILE`.
#include "logic/logic_layer.h" /// `logic_layer.logging_enabled`.


/* Predef */

void log_success(const char *const text);
void log_error  (const char *const text);
void log_warning(const char *const text);
void log_SDL_error  (const char *const text);
void log_SDL_warning(const char *const text);


/* Body */

void log_success(const char *const text)
{
    if (text == NULL)
        return;
    if (! logic_layer.logging_enabled)
        return;

    FILE *const log_file = fopen(LOG_FILE_PATH, "a");
    fprintf(log_file, "~ [SUCCESS] %s.\n", text);
    fclose(log_file);
    return;
}


void log_error(const char *const text)
{
    if (text == NULL)
        return;
    if (! logic_layer.logging_enabled)
        return;

    FILE *const log_file = fopen(LOG_FILE_PATH, "a");
    fprintf(log_file, "~ [ERROR] %s.\n", text);
    fclose(log_file);
    return;
}


void log_warning(const char *const text)
{
    if (text == NULL)
        return;
    if (! logic_layer.logging_enabled)
        return;

    FILE *const log_file = fopen(LOG_FILE_PATH, "a");
    fprintf(log_file, "~ [WRNNG] %s.\n", text);
    fclose(log_file);
    return;
}


void log_SDL_error(const char *const text)
{
    if (text == NULL)
        return;
    if (! logic_layer.logging_enabled)
        return;

    FILE *const log_file = fopen(LOG_FILE_PATH, "a");
    fprintf(log_file, "~ [ERROR] %s: \"%s\".\n", text, SDL_GetError());
    fclose(log_file);
    return;
}


void log_SDL_warning(const char *const text)
{
    if (text == NULL)
        return;
    if (! logic_layer.logging_enabled)
        return;

    FILE *const log_file = fopen(LOG_FILE_PATH, "a");
    fprintf(log_file, "~ [WRNNG] %s: \"%s\".\n", text, SDL_GetError());
    fclose(log_file);
    return;
}