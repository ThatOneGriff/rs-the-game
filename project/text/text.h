#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <SDL3/SDL.h> /// SDL3.
#include <SDL3_ttf/SDL_ttf.h> /// SDL3_TTF.

#include "border.h"       /// Text w/ borders.
#include "../resources.h" /// Font path(s).
#include "../graphics/graphics_layer.h" /// `SDL_CreateTextureFromSurface()`.
#include "../graphics/texture.h" /// Rendering text into a texture.


/* Predef */

struct Texture create_text(const char* text, const SDL_Color inner_color, const SDL_Color outer_color, const unsigned int size, const unsigned int border_thickness, int* exit_code);


/* Body */

struct Texture create_text(const char* text, const SDL_Color inner_color, const SDL_Color outer_color, const unsigned int size, const unsigned int border_thickness, int* exit_code)
{
    struct Texture result;

    /* Checking args */

    if (size == 0)
    {
        print_error("`create_text()`: `size` is 0", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (exit_code == NULL)
        print_warning("`create_text()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);

    /* Creating the text */

    SDL_Surface* text_surf = create_bordered_text_surface(text, size, border_thickness, inner_color, outer_color);
    if (text_surf == NULL)
    {
        print_error("`create_text()`: couldn't create `text_surf`", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    result.texture = SDL_CreateTextureFromSurface(graphics_layer.renderer, text_surf);
    if (result.texture == NULL)
    {
        print_error("`create_text()`: couldn't create a resulting texture from `text_surf`", IS_SDL_ERROR);
        SDL_DestroySurface(text_surf);
        text_surf = NULL;
        *exit_code = EXIT_FAILURE;
        return result;
    }
    result.rect.w = (float)text_surf->w;
    result.rect.h = (float)text_surf->h;

    SDL_DestroySurface(text_surf);
    text_surf = NULL;
    
    /// Centering the result on the screen. TODO: should instead be passed as a parameter.
    result.rect.x = (float)(WINDOW_WIDTH  - result.rect.w) / 2;// * rand_percent(95, 105);
    result.rect.y = (float)(WINDOW_HEIGHT - result.rect.h) / 2;// * rand_percent(95, 105);

    *exit_code = EXIT_SUCCESS;
    return result;
}


/*void move_text(struct Text_IMG* target)
{
    if (target == NULL)
    {
        print_error("`move_text`: `target` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    else if (target->velocity == 0)
        return;
    
    /// A direction flip only occurs if we're about to cross the "border",
    /// WHILE moving towards said border.
    /// Reason: when FPS is in thousands, we flip the direction, move away from the border,
    /// and register a "crossing" the next frame - which results in undesired direction changes.
    if ((target->direction.x == +1 && ((target->rect.x + target->rect.w + target->velocity) >= WINDOW_WIDTH))
     || (target->direction.x == -1 && (target->rect.x - target->velocity) <= 0))
        target->direction.x = -target->direction.x;
    if ((target->direction.y == +1 && ((target->rect.y + target->rect.h + target->velocity) >= WINDOW_HEIGHT))
     || (target->direction.y == -1 && (target->rect.y - target->velocity) <= 0))
        target->direction.y = -target->direction.y;
    
    const float x_move = (float)target->direction.x * (float)target->velocity * movement_FPS_manager.fraction_of_target_delta_elapsed;
    const float y_move = (float)target->direction.y * (float)target->velocity * movement_FPS_manager.fraction_of_target_delta_elapsed;
    target->rect.x += x_move;
    target->rect.y += y_move;
}*/

#endif /// TEXT_H