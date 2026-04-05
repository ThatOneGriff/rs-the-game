#pragma once
#ifndef LOGS_H
#define LOGS_H


/* Predef */

void log_success(const char *const text);
void log_error  (const char *const text);
void log_warning(const char *const text);
void log_SDL_error  (const char *const text);
void log_SDL_warning(const char *const text);


#endif /// LOGS_H