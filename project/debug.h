#ifndef DEBUG_H
#define DEBUG_H

#define UNUSED(expr) do { (void)(expr); } while (0)


/* Predef */

void print_success(const char *const text);
void print_error  (const char *const text);
void print_warning(const char *const text);
void print_SDL_error  (const char *const text);
void print_SDL_warning(const char *const text);


#endif /// DEBUG_H