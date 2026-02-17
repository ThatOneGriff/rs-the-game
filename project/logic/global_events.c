/* Related headers */
#include "global_events.h"

/* SDL3 */
#include <SDL3/SDL.h> /// `SDL_Event`, `SDL_Keycode`.

/* C headers */
#include <stdbool.h> /// For `logic_layer` manipulation.
#include <stdio.h>   /// I/O.
#include <stdlib.h>  /// Exit codes.

/* Helper headers */
#include "logic_layer.h"           /// Exiting the game.
#include "../debug.h"              /// Some controls should only be available `#ifdef DEBUG`.
#include "../helpers/helpers.h"    /// Colored output.
#include "../resources.h"          /// Hot resource reload.
#include "../audio/music_loader.h" /// Music manipulation.


/* Predef */

void process_global_events  (const SDL_Event   event);
void process_global_keyboard(const SDL_Keycode event_key);


/* Body */

/// Receives its `event` as argument because it gets called from scene-related event functions
/// (doesn't read an event on its own because of its lower priority).
void process_global_events(const SDL_Event event)
{
    switch (event.type)
    {
    /// Quit
    case SDL_EVENT_QUIT:
        logic_layer.game_is_running = false;
        break;
    }
    return;
}


void process_global_keyboard(const SDL_Keycode event_key)
{
    int exit_code = EXIT_SUCCESS;
    switch(event_key)
    {
    case SDLK_R:
        load_global_resources(&exit_code);
        if (exit_code == EXIT_SUCCESS)
            print_success("Hot resource reload; switch to next scene and back to see the effect");
        else /// First multi-line error message btw.
            print_error("[!!!] Hot resource reload FAILED:\n- Scene reloading will result in a crash;\n- You may try to fix invalid paths and reload again", NON_SDL_ERROR);
        break;
    }
    return;
}