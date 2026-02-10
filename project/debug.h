#pragma once
#ifndef DEBUG_H
#define DEBUG_H

/* Headers */
#include <stdbool.h>  /// `bool` args for `print_*()`.

#define UNUSED(expr) do { (void)(expr); } while (0)
#define  IS_SDL_ERROR true
#define NON_SDL_ERROR false


/* Predef */

void print_success(const char *const text);
void print_error  (const char *const text, const bool is_SDL_error);
void print_warning(const char *const text, const bool is_SDL_error);


#endif /// DEBUG_H