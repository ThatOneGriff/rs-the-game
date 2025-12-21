#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <SDL3/SDL.h> /// SDL3.
#include <SDL3_ttf/SDL_ttf.h> /// SDL3_TTF.

#include "border.h"       /// Text w/ borders.
#include "../resources.h" /// Font path(s).
#include "../graphics/fps.h"            /// Text movement (NOTE: text movement may be redundant)
#include "../graphics/graphics_layer.h" /// `SDL_CreateTextureFromSurface()`
#include "../helpers/helpers.h" /// `struct Vec2` for text movement (NOTE 2: text movement may be redundant)
#include "../helpers/random.h"  /// Randomized text direction (NOTE: text movement may be redundant)


/* Struct */

struct Text_IMG
{
    SDL_Texture* texture;
    SDL_FRect    rect;
    struct Vec2 direction;
    float velocity;
};


/* Predef */

struct Text_IMG create_text(              const char* text,     const SDL_Color color,     const unsigned int size,     const unsigned int border_thickness, const float movement_velocity, int* exit_code);
void free_text  (struct Text_IMG* target);
void rebake_text(struct Text_IMG* target, const char* new_text, const SDL_Color new_color, const unsigned int new_size, const unsigned int new_border_thickness, int* exit_code);

void move_text  (struct Text_IMG* target);


/* Body */

struct Text_IMG create_text(const char* text, const SDL_Color color, const unsigned int size, const unsigned int border_thickness, const float movement_velocity, int* exit_code)
{
    struct Text_IMG result;
    if (size == 0)
    {
        print_error("`create_text()`: `size` is 0", NON_SDL_ERROR);
        return result;
    }

    /// `0` arg for auto-determined length.
    SDL_Surface* text_surf = create_bordered_text_surface(text, size, border_thickness, (SDL_Color){255, 255, 255, 255}, color);
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
    
    /// Centering on the screen with a 5% wiggle room. TODO: should instead be passed as a parameter.
    result.rect.x = (float)(WINDOW_WIDTH  - result.rect.w) / 2 * rand_percent(95, 105);
    result.rect.y = (float)(WINDOW_HEIGHT - result.rect.h) / 2 * rand_percent(95, 105);

    if (movement_velocity != 0)
    {
        result.direction.x = (int)randint_except(1,3,2) - 2; /// -1, 1
        result.direction.y = (int)randint_except(1,3,2) - 2; /// -1, 1
        result.velocity = movement_velocity;
    }

    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_text(struct Text_IMG* target)
{
    SDL_DestroyTexture(target->texture);
    target->texture = NULL;
    target->rect = (SDL_FRect){0.0, 0.0, 0.0, 0.0};
    target->direction = (struct Vec2){0, 0};
}


void rebake_text(struct Text_IMG* target, const char* new_text, const SDL_Color new_color, const unsigned int new_size, const unsigned int new_border_thickness, int* exit_code)
{
    if (new_size == 0)
    {
        print_error("`rebake_text()`: `new_size` parameter set as 0", NON_SDL_ERROR);
        return;
    }

    /// `0` arg for auto-determined length.
    SDL_Surface* text_surf = create_bordered_text_surface(new_text, new_size, new_border_thickness, (SDL_Color){255, 255, 255, 255}, new_color);
    if (text_surf == NULL)
    {
        print_error("`rebake_text()`: couldn't create `text_surf`", IS_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    target->texture = SDL_CreateTextureFromSurface(graphics_layer.renderer, text_surf);
    if (target->texture == NULL)
    {
        print_error("`rebake_text()`: couldn't create a resulting texture from `text_surf`", IS_SDL_ERROR);
        SDL_DestroySurface(text_surf);
        text_surf = NULL;
        *exit_code = EXIT_FAILURE;
        return;
    }
    target->rect.w = (float)text_surf->w;
    target->rect.h = (float)text_surf->h;

    SDL_DestroySurface(text_surf);
    text_surf = NULL;

    *exit_code = EXIT_SUCCESS;
}


void move_text(struct Text_IMG* target)
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
}

#endif /// TEXT_H