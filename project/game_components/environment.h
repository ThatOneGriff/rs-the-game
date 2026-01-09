#pragma once
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_DestroyTexture()`.
#include <SDL3_image/SDL_image.h> /// `IMG_Load()`.

/* C headers */
#include <stdlib.h>               /// `*alloc()`.

/* Related headers */
#include "move_component.h"       /// Moving textures.

/* Helpers */
#include "../debug.h"                   /// Error printing.
#include "../deinit_stack.h"            /// Deinitialization stack.
#include "../resources.h"               /// Null texture.
#include "../graphics/graphics_layer.h" /// `graphics_layer`.


/* Struct */

/// This is a combination of `Shifting_Texture` and `Multi_Texture`:
/// a set of textures, going along a path, randomized with each iteration.
struct Environment
{
    SDL_Texture**          textures;
    struct Move_Component* move_component;
    size_t count;

    /// `cur_texture_i` and `rects` KIND OF make an object.
    SDL_FRect* rects; /// Exists solely for manipulation by `move_component`.
    size_t*    cur_texture_indexes;
    /// TODO: ptr to a function (to check collisions with traffic).
    /// TODO: a way to render layers correctly.
};


/* Predef */

struct Environment        new_environment(const char** texture_paths, const size_t count, int* exit_code);
void couple_move_component_to_environment(struct Environment* to, struct Move_Component* move_component, int* exit_code);
void                     free_environment(struct Environment* target);

void render_environment(const struct Environment* target);


/* Body */

struct Environment new_environment(const char** texture_paths, const size_t count, int* exit_code)
{
    struct Environment result;
    result.textures       = NULL;
    result.move_component = NULL;
    result.count = 0; /// Temporary value to be changed once memory is successfully allocated.
    result.cur_texture_indexes = NULL;
    result.rects               = NULL;

    /// Param checking
    if (exit_code == NULL)
        print_warning("`new_environment()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (count == 0)
    {
        print_warning("`new_environment()`: `max_count` arg is 0. `Environment` is not dynamic-memory", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (texture_paths == NULL)
    {
        print_error("`new_environment()`: `texture_paths` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Object creation
    /// Textures (memory)
    result.textures = calloc(count, sizeof(SDL_Texture*));
    if (result.textures == NULL)
    {
        print_error("`new_environment()`: couldn't allocate memory for textures", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(2, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`new_environment()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, result.textures, (void (*)(void*))free);
    /// Textures (loading)
    for (size_t i = 0; i < count; i++)
    {
        result.textures[i] = IMG_LoadTexture(graphics_layer.renderer, texture_paths[i]);
        if (result.textures[i] == NULL)
        {
            if (NULL_TEXTURE != NULL)
            {
                print_warning("`new_multi_texture()`: couldn't load the texture, replaced with null texture", IS_SDL_ERROR);
                result.textures[i] = NULL_TEXTURE;
            }
            else
            {
                print_error("`new_multi_texture()`: couldn't load the texture, and null texture is empty", IS_SDL_ERROR);
                flush_deinit_stack(&deinit_stack);
                *exit_code = EXIT_FAILURE;
                return result;
            }
        }
        else
            add_to_deinit_stack(&deinit_stack, result.textures[i], (void (*)(void*))SDL_DestroyTexture);
    }

    result.rects = calloc(count, sizeof(SDL_FRect)); /// We don't need to actually fill them, as that's `move_component`'s job.
    if (result.rects == NULL)
    {
        print_error("`new_multi_texture()`: couldn't allocate memory to rects", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    add_to_deinit_stack(&deinit_stack, result.rects, (void (*)(void*))free);

    result.cur_texture_indexes = calloc(count, sizeof(size_t)); /// We don't need to actually fill them, as that's `move_component`'s job.
    if (result.cur_texture_indexes == NULL)
    {
        print_error("`new_multi_texture()`: couldn't allocate memory to `cur_texture_indexes`", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    for (size_t i = 0; i < count; i++)
        result.cur_texture_indexes[i] = randint(0, count-1); /// Randomized textures.

    result.count = count;
    free_deinit_stack(&deinit_stack); /// `free()` because all members are to be used later.
    *exit_code = EXIT_SUCCESS;
    return result;
}


void couple_move_component_to_environment(struct Environment* to, struct Move_Component* move_component, int* exit_code)
{
    /// Arg checking
    if (exit_code == NULL)
        print_warning("`couple_move_component_to_environment()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (to == NULL)
    {
        print_error("`couple_move_component_to_environment()`: `to` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (move_component == NULL)
    {
        print_error("`couple_move_component_to_environment()`: `move_component` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Coupling
    couple_move_component(move_component, to->rects, to->count, RANDOMIZED_POSITIONS, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`couple_move_component_to_environment()`: coupling failed", NON_SDL_ERROR);
        return;
    }
    to->move_component = move_component;

    *exit_code = EXIT_SUCCESS;
    return;
}


void free_environment(struct Environment* target)
{
    if (target == NULL)
        return;
    
    if (target->textures != NULL)
    {
        for (size_t i = 0; i < target->count; i++)
            if (target->textures[i] != NULL_TEXTURE)
                SDL_DestroyTexture(target->textures[i]);
        free(target->textures);
        target->textures = NULL;
    }
    
    if (target->rects != NULL)
    {
        free(target->rects);
        target->rects = NULL;
    }
    
    if (target->cur_texture_indexes != NULL)
    {
        free(target->cur_texture_indexes);
        target->cur_texture_indexes = NULL;
    }

    if (target->move_component != NULL)
    {
        free_move_component(target->move_component);
        target->move_component = NULL;
    }

    target->count = 0;
    return;
}


/* Functions */

void render_environment(const struct Environment* target)
{
    if (target == NULL || target->textures == NULL || target->rects == NULL || target->cur_texture_indexes == NULL || target->count == 0)
    {
        print_error("`render_environment()`: `target` or its members are invalid", NON_SDL_ERROR);
        return;
    }

    if (target->move_component != NULL)
        move_all_rects(target->move_component);
    
    for (size_t i = 0; i < target->count; i++)
    {
        if (target->rects[i].x + target->rects[i].w <= 0 ||
            target->rects[i].y + target->rects[i].h <= 0 ||
            target->rects[i].x >= RENDER_WIDTH ||
            target->rects[i].y >= RENDER_HEIGHT)
        {
            print_warning("`render_environment()`: texture rendering out of bounds", NON_SDL_ERROR);
            continue; /// While it's not an error, why waste a draw call on something not seen anyway?
        }
        else
            SDL_RenderTexture(graphics_layer.renderer, target->textures[target->cur_texture_indexes[i]], NULL, &target->rects[i]);
    }
}

#endif /// ENVIRONMENT_H