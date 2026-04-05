/* Headers */
#include <stdlib.h>            /// `FILE`.
#include "logic/logic_layer.h" /// `logic_layer.logging_enabled`.

#define LOG_FILE_PATH "./rs-the-game.log"


/* Predef */

void log_success(const char *const text);
void log_error  (const char *const text);
void log_warning(const char *const text);
void log_SDL_error  (const char *const text);
void log_SDL_warning(const char *const text);


/* Body */

