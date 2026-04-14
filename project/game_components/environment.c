/* Related header */
#include "environment.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// `SDL_DestroyTexture()`.
#include <SDL3_image/SDL_image.h> /// `IMG_Load()`.

/* C headers */
#include <stdlib.h>               /// `*alloc()`.
#include <string.h>               /// `memset()`.

/* Related headers */
#include "movement/move_component.h"    /// Moving textures.

/* Helpers */
#include "../debug.h"                   /// Error printing.
#include "../deinit_stack.h"            /// Deinitialization stack.
#include "../resources.h"               /// Null texture.
#include "../helpers/helpers.h"         /// `free_ptr_arr()`.
#include "../helpers/random.h"          /// `randint()`.
#include "../graphics/graphics_layer.h" /// `graphics_layer`.


/* === IN CASE YOU END UP HERE ===
I wholeheartedly hate this code. Sorry. */


/* Predef */

struct Environment        new_environment(char **const texture_paths, const unsigned short texture_count, const unsigned short object_count, int *const exit_code);
void couple_move_component_to_environment(struct Environment *const to, struct Move_Component *const move_component, const struct Vec2 max_offset, int *const exit_code);
void                     free_environment(struct Environment *const target);
void        render_environment(const struct Environment *const target);
void partly_render_environment(const struct Environment *const target, const unsigned short min_path_pt, unsigned short max_path_pt);


/* Body */

struct Environment new_environment(char **const texture_paths, const unsigned short texture_count, const unsigned short object_count, int *const exit_code)
{
    struct Environment result = {0};

    /// Param checking
    if (exit_code == NULL)
        print_warning("`new_environment()`: `exit_code` arg is `NULL`");
    if (object_count == 0 || texture_count == 0)
    {
        print_warning("`new_environment()`: `object_count` or `texture_count` arg is 0. `Environment` is not dynamic-memory");
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (texture_paths == NULL)
    {
        print_error("`new_environment()`: `texture_paths` arg is `NULL`");
        *exit_code = EXIT_FAILURE;
        return result;
    }

    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(3+texture_count, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`new_environment()`: couldn't instance a deinitialization stack");
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)texture_paths, texture_count);
        return result;
    }

    /// Object creation
    /// Textures (memory)
    result.textures = calloc(texture_count, sizeof(SDL_Texture*));
    if (result.textures == NULL)
    {
        print_error("`new_environment()`: couldn't allocate memory for textures");
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)texture_paths, texture_count);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    add_to_deinit_stack(&deinit_stack, result.textures, (void (*)(void*))free);
    /// Textures (loading)
    for (unsigned short i = 0; i < texture_count; i++)
    {
        result.textures[i] = IMG_LoadTexture(graphics_layer.renderer, texture_paths[i]);
        if (result.textures[i] == NULL)
        {
            if (NULL_TEXTURE != NULL)
            {
                print_SDL_warning("`new_environment()`: couldn't load the texture, replaced with null texture");
                result.textures[i] = NULL_TEXTURE;
            }
            else
            {
                print_SDL_error("`new_environment()`: couldn't load the texture, and null texture is empty");
                flush_deinit_stack(&deinit_stack);
                free_ptr_arr((void**)texture_paths, texture_count);
                *exit_code = EXIT_FAILURE;
                return result;
            }
        }
        else
            add_to_deinit_stack(&deinit_stack, result.textures[i], (void (*)(void*))SDL_DestroyTexture);
    }
    result.texture_count = texture_count;

    result.rects = calloc(object_count, sizeof(SDL_FRect)); /// We don't need to actually fill them, as that's `move_component`'s job.
    if (result.rects == NULL)
    {
        print_error("`new_environment()`: couldn't allocate memory to rects");
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)texture_paths, texture_count);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    add_to_deinit_stack(&deinit_stack, result.rects, (void (*)(void*))free);

    result.cur_texture_indexes = calloc(object_count, sizeof(unsigned short)); /// We don't need to actually fill them, as that's `move_component`'s job.
    if (result.cur_texture_indexes == NULL)
    {
        print_error("`new_environment()`: couldn't allocate memory to `cur_texture_indexes`");
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)texture_paths, texture_count);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    for (unsigned short i = 0; i < object_count; i++)
        result.cur_texture_indexes[i] = (unsigned short)randint(0, (unsigned)texture_count-1); /// Randomized textures.

    result.object_count = object_count;
    free_deinit_stack(&deinit_stack); /// `free()` because all members are to be used later.
    //free_ptr_arr((void**)texture_paths, texture_count);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void couple_move_component_to_environment(struct Environment *const to, struct Move_Component *const move_component, const struct Vec2 max_offset, int *const exit_code)
{
    /// Arg checking
    if (exit_code == NULL)
        print_warning("`couple_move_component_to_environment()`: `exit_code` arg is `NULL`");
    if (to == NULL)
    {
        print_error("`couple_move_component_to_environment()`: `to` arg is `NULL`");
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (move_component == NULL)
    {
        print_error("`couple_move_component_to_environment()`: `move_component` arg is `NULL`");
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Coupling
    couple_move_component(move_component, to->rects, to->object_count, max_offset, RANDOMIZED_POSITIONS, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`couple_move_component_to_environment()`: coupling failed");
        return;
    }
    to->move_component = move_component;

    *exit_code = EXIT_SUCCESS;
    return;
}


void free_environment(struct Environment *const target)
{
    if (target == NULL)
        return;
    
    if (target->textures != NULL)
    {
        for (unsigned short i = 0; i < target->texture_count; i++)
            if (target->textures[i] != NULL_TEXTURE)
                SDL_DestroyTexture(target->textures[i]);
        free(target->textures);
    }
    if (target->rects != NULL)
        free(target->rects);
    if (target->cur_texture_indexes != NULL)
        free(target->cur_texture_indexes);
    if (target->move_component != NULL)
        free_move_component(target->move_component);
    
    memset(target, 0, sizeof *target);
    return;
}


/* Functions */

void render_environment(const struct Environment *const target)
{
    if (target == NULL || target->textures == NULL || target->rects == NULL || target->cur_texture_indexes == NULL || target->texture_count == 0 || target->object_count == 0)
    {
        print_error("`render_environment()`: `target` or its members are invalid");
        return;
    }

    if (target->move_component == NULL)
    {
        print_error("`render_environment()`: `target->move_component` is not coupled");
        return;
    }
    
    move_all_rects(target->move_component);
    /// Rendering in path point order.
    /// [FALSE FOR NOW] Only 1 texture per path point is actually rendered to avoid visual cluttering.
    for (unsigned short path_pt = 0; path_pt < target->move_component->path.pt_count; path_pt++)
    {
        for (unsigned short i = 0; i < target->object_count; i++)
        {
            if (target->move_component->rects_pt_indices[i] != path_pt)
                continue;
            
            if (target->rects[i].x + target->rects[i].w <= 0 ||
                target->rects[i].y + target->rects[i].h <= 0 ||
                target->rects[i].x >= RENDER_WIDTH ||
                target->rects[i].y >= RENDER_HEIGHT
            )
                continue; /// Out of bounds.
            
            SDL_RenderTexture(graphics_layer.renderer, target->textures[target->cur_texture_indexes[i]], NULL, &target->rects[i]);
            break;
        }
    }
    
    return;
}


/// REDO according to DRY principle.
void partly_render_environment(const struct Environment *const target, const unsigned short min_path_pt, unsigned short max_path_pt)
{
    if (target == NULL || target->textures == NULL || target->rects == NULL || target->cur_texture_indexes == NULL || target->texture_count == 0 || target->object_count == 0)
    {
        print_error("`render_environment()`: `target` or its members are invalid");
        return;
    }

    if (max_path_pt < min_path_pt)
    {
        print_error("`render_environment()`: `max_path_pt` < `min_path_pt`");
        return;
    }

    if (target->move_component == NULL)
    {
        print_error("`render_environment()`: `target->move_component` is not coupled");
        return;
    }
    
    /// Rendering in path point order.
    /// [FALSE FOR NOW] Only 1 texture per path point is actually rendered to avoid visual cluttering.
    if (max_path_pt > target->move_component->path.pt_count)
        max_path_pt = target->move_component->path.pt_count-1;
    for (unsigned short path_pt = min_path_pt; path_pt <= max_path_pt; path_pt++)
    {
        for (unsigned short i = 0; i < target->object_count; i++)
        {
            if (target->move_component->rects_pt_indices[i] != path_pt)
                continue;
            
            if (target->rects[i].x + target->rects[i].w <= 0 ||
                target->rects[i].y + target->rects[i].h <= 0 ||
                target->rects[i].x >= RENDER_WIDTH ||
                target->rects[i].y >= RENDER_HEIGHT
            )
                continue; /// Out of bounds.
            
            SDL_RenderTexture(graphics_layer.renderer, target->textures[target->cur_texture_indexes[i]], NULL, &target->rects[i]);
            break;
        }
    }
    
    return;
}