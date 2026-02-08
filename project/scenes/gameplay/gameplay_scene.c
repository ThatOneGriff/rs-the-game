/* Related header */
#include "gameplay_scene.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* Helper headers */
#include <string.h>                   /// `memset()`.
#include "../../debug.h"              /// Error printing.
#include "../../deinit_stack.h"       /// Deinitialization stack.
#include "../../resources.h"          /// Null texture.
#include "../../helpers/helpers.h"    /// `free_ptr_arr()`.
#include "../../helpers/random.h"     /// Random.
#include "../../logic/logic_layer.h"  /// `time_tick_ms`.

/* Graphics & components */
#include "../car.h"                                          /// `*car_ptr` (i.e. player).
#include "pause_screen.h"                                    /// Pause screen.
#include "traffic_manager.h"                                 /// Traffic manager.
#include "../../game_components/environment.h"               /// `struct Environment`.
#include "../../game_components/graphics/multi_texture.h"    /// Multi-textures.
#include "../../game_components/graphics/shifting_texture.h" /// Shifting textures.
#include "../../game_components/graphics/texture.h"          /// Textures.
#include "../../game_components/movement/move_component.h"   /// `struct Move_Component`.
#include "../../graphics/fps.h"                              /// FPS is needed for moving clouds (TEMP).
#include "../../graphics/graphics_layer.h"                   /// `graphics_layer.renderer`.


/* Predef */

struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code);
void                  free_gameplay_scene(struct Gameplay_Scene* target);
void                  render_gameplay_scene(struct Gameplay_Scene* target);
void update_points(struct Gameplay_Scene* target);


/* Body */

struct Gameplay_Scene load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code)
{
    struct Gameplay_Scene result = {0};

    /// Param checking
    if (exit_code == NULL)
        print_warning("`load_gameplay_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (path == NULL)
    {
        print_error("`load_gameplay_scene()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    if (car_ptr == NULL)
    {
        print_error("`load_gameplay_scene()`: `car_ptr` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    else
        result.car_ptr = car_ptr;

    /// Reading the file
    char** scene_data = read_file_by_line(path, GAMEPLAY_DATA_LINES);
    if (scene_data == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't read data from file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return result;
    }
    
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(7, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }

    /// Sky
    result.sky_bg = IMG_LoadTexture(graphics_layer.renderer, scene_data[0]);
    if (result.sky_bg == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`load_gameplay_scene()`: couldn't load the sky texture, replaced with null texture", IS_SDL_ERROR);
            result.sky_bg = NULL_TEXTURE;
        }
        else
        {
            print_error("`load_gameplay_scene()`: couldn't load the sky texture, and null texture is empty", IS_SDL_ERROR);
            free_deinit_stack(&deinit_stack);
            free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
            *exit_code = EXIT_FAILURE;
            return result;
        }
    }
    if (result.sky_bg != NULL_TEXTURE)
        add_to_deinit_stack(&deinit_stack, result.sky_bg, (void (*)(void*))SDL_DestroyTexture);

    /// Ground
    result.ground = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 4, 150, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the ground texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    result.ground.freeze_shifting = true;
    add_to_shifting_texture(&result.ground, scene_data[1], exit_code); /// TODO: find a way
    add_to_shifting_texture(&result.ground, scene_data[2], exit_code); /// to check `exit_code`
    add_to_shifting_texture(&result.ground, scene_data[3], exit_code); /// w/o obnoxious
    add_to_shifting_texture(&result.ground, scene_data[4], exit_code); /// `if() {}` blocks.
    add_to_deinit_stack(&deinit_stack, &result.ground, (void (*)(void*))free_shifting_texture);

    /// Road
    result.road = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 3, 100, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the road texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    result.road.freeze_shifting = true;
    add_to_shifting_texture(&result.road, scene_data[5], exit_code);
    add_to_shifting_texture(&result.road, scene_data[6], exit_code);
    add_to_shifting_texture(&result.road, scene_data[7], exit_code);
    add_to_deinit_stack(&deinit_stack, &result.road, (void (*)(void*))free_shifting_texture);
    
    /// Stripes
    result.stripes = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 3, 100, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the stripes texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    result.stripes.freeze_shifting = true;
    add_to_shifting_texture(&result.stripes, scene_data[8], exit_code);
    add_to_shifting_texture(&result.stripes, scene_data[9], exit_code);
    add_to_shifting_texture(&result.stripes, scene_data[10], exit_code);
    add_to_deinit_stack(&deinit_stack, &result.stripes, (void (*)(void*))free_shifting_texture);

    /// Trees
    result.trees = new_environment((char*[]){scene_data[11], scene_data[12], scene_data[13], scene_data[14]}, 4, 7, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the trees", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    /// - Tree movement
    struct Path tree_path = new_path(
        (SDL_FRect[]){{80,75, 7, 7},  {75,70,10,10},  {65,65,15,15},
                      {55,60,22,22},  {35,60,30,30},  /*{10,60,40,40},*/
                       {5,55,45,45}, {-30,50,60,60}, {-60,45,75,75}},
                       8, exit_code /// TODO: exit code check.
    );
    struct Move_Component* tree_move_component = malloc(sizeof(struct Move_Component));
    *tree_move_component = init_move_component(tree_path, 150, true, exit_code); /// TODO: exit code check.
    couple_move_component_to_environment(&result.trees, tree_move_component, vec2(10,0), exit_code); /// TODO: exit code check.
    add_to_deinit_stack(&deinit_stack, &result.trees, (void (*)(void*))free_environment); /// Don't delete. More elements will be added later

    /// TODO: checks.
    result.clouds[0] = load_texture(scene_data[15], (SDL_FRect){(float)randint(0, RENDER_WIDTH-30),  5, 30, 10}, exit_code);
    result.clouds[1] = load_texture(scene_data[16], (SDL_FRect){(float)randint(0, RENDER_WIDTH-32), 12, 32, 15}, exit_code);
    result.clouds[2] = load_texture(scene_data[17], (SDL_FRect){(float)randint(0, RENDER_WIDTH-23), 24, 23,  8}, exit_code);
    result.clouds[3] = load_texture(scene_data[18], (SDL_FRect){(float)randint(0, RENDER_WIDTH-41), 29, 41,  8}, exit_code);
    result.clouds[4] = load_texture(scene_data[19], (SDL_FRect){(float)randint(0, RENDER_WIDTH-36), 34, 36,  8}, exit_code);
    result.clouds[5] = load_texture(scene_data[15], (SDL_FRect){(float)randint(0, RENDER_WIDTH-30),  5, 30, 10}, exit_code);
    result.clouds[6] = load_texture(scene_data[16], (SDL_FRect){(float)randint(0, RENDER_WIDTH-32), 12, 32, 15}, exit_code);
    result.clouds[7] = load_texture(scene_data[17], (SDL_FRect){(float)randint(0, RENDER_WIDTH-23), 24, 23,  8}, exit_code);
    result.clouds[8] = load_texture(scene_data[18], (SDL_FRect){(float)randint(0, RENDER_WIDTH-41), 29, 41,  8}, exit_code);
    result.clouds[9] = load_texture(scene_data[19], (SDL_FRect){(float)randint(0, RENDER_WIDTH-36), 34, 36,  8}, exit_code);

    /// Pause screen
    result.pause_screen = init_pause_screen(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't create pause screen", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    add_to_deinit_stack(&deinit_stack, &result.pause_screen, (void (*)(void*))free_pause_screen);
    /// Below block: DON'T MOVE out of this function!!! Some memory fuckery will happen.
    /// - Pause screen: setting button neighbours.
    ///                    | Target                                     | Up                                      | Down                                       |Left |Right
    add_neighbors_to_button(&result.pause_screen.          close_button, NULL,                                     &result.pause_screen.       continue_button, NULL, NULL);
    add_neighbors_to_button(&result.pause_screen.       continue_button, &result.pause_screen.       close_button, &result.pause_screen.   quit_to_menu_button, NULL, NULL);
    add_neighbors_to_button(&result.pause_screen.   quit_to_menu_button, &result.pause_screen.    continue_button, &result.pause_screen.quit_to_desktop_button, NULL, NULL);
    add_neighbors_to_button(&result.pause_screen.quit_to_desktop_button, &result.pause_screen.quit_to_menu_button, NULL,                                        NULL, NULL);
    result.pause_screen.curr_button = &result.pause_screen.continue_button;

    init_traffic_manager(5, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't initialize traffic manager", NON_SDL_ERROR);
        free_traffic_manager();
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return result;
    }
    
    char personal_best_text[10];
    sprintf(personal_best_text, "PB: %d", PERSONAL_BEST);
    result.personal_best_text = create_text(personal_best_text, (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(5, 5), 15, 1, exit_code);
    add_to_deinit_stack(&deinit_stack, &result.personal_best_text, (void (*)(void*))free_texture);

    result.curr_points_text = create_text("Pts: 0", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(5, 20), 15, 1, exit_code);
    
    free_deinit_stack(&deinit_stack); /// `free` because those resources will be used.
    free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
    *exit_code = EXIT_SUCCESS;
    return result;
}


void free_gameplay_scene(struct Gameplay_Scene* target)
{
    if (target == NULL)
        return;
    
    target->car_ptr->coords.x     = center_x(target->car_ptr->coords.w);
    target->car_ptr->base_texture = 2;
    target->car_ptr->latest_turn_start     = 0;
    target->car_ptr->latest_turn_end       = 0;
    target->car_ptr->prev_turn_direction_x = 0;
    
    free_texture(&target->personal_best_text);
    free_pause_screen(&target->pause_screen);
    for (size_t i = 0; i < 10; i++)
        free_texture(&target->clouds[i]);
    free_environment     (&target->trees);
    free_shifting_texture(&target->stripes);
    free_shifting_texture(&target->road);
    free_shifting_texture(&target->ground);
    if (target->sky_bg != NULL && target->sky_bg != NULL_TEXTURE)
    {
        SDL_DestroyTexture(target->sky_bg);
        target->sky_bg = NULL;
    }

    memset(target, 0, sizeof *target);
    return;
}


void render_gameplay_scene(struct Gameplay_Scene* target)
{
    /// Param checking
    if (target == NULL || target->sky_bg == NULL) /// TODO: check all members.
    {
        print_error("`render_gameplay_scene()`: `target` arg or one of its members is `NULL`", NON_SDL_ERROR);
        return;
    }

    if (target->pause_screen.is_open)
    {
        render_pause_screen(&target->pause_screen);
        return;
    }
    
    /// Moving stuff
    if (target->is_driving)
    {
        move_all_rects(target->trees.move_component);
        target->ground. freeze_shifting = false;
        target->road.   freeze_shifting = false;
        target->stripes.freeze_shifting = false;
        if ((target->   start_tick != 0)
         && (logic_layer.curr_tick - target->start_tick >= 2000)) /// Pause before traffic starts coming.
            move_traffic(MOVE_NORMAL);
    }
    else if (! target->is_driving)
    {
        target->ground. freeze_shifting = true;
        target->road.   freeze_shifting = true;
        target->stripes.freeze_shifting = true;
        if (target->start_tick != 0) /// Checks that we're not on the start.
        {
            move_traffic(MOVE_REVERSE);
            if (target->crash_tick == 0)
                target->crash_tick = logic_layer.curr_tick;
            else if (target->crash_tick != ULONG_LONG_MAX && logic_layer.curr_tick - target->crash_tick >= 2000)
            {
                target->crash_tick = ULONG_LONG_MAX; /// So that the pause menu opens exactly once.
                show_pause_screen(&target->pause_screen);
            }
        }
    }

    /// Rendering
    SDL_RenderTexture(graphics_layer.renderer, target->sky_bg, NULL, NULL);
    for (size_t i = 0; i < 10; i++)
    {
        /// Just a bunch of magic numbers I got after playing around.
        target->clouds[i].rect.x += (float)CLOUD_VELOCITY * (float)FPS_manager.delta_ns / SEC_IN_NS / target->clouds[i].rect.w;
        if (target->clouds[i].rect.x >= RENDER_WIDTH - 1)
            target->clouds[i].rect.x = - target->clouds[i].rect.w + 1;
        render_texture(&target->clouds[i]);
    }
    if ((target->is_driving && (logic_layer.curr_tick - target->start_tick >= 2000)) || target->crash_tick != 0)
        render_traffic_on_pts(0, 0, NULL, NULL, NULL);
    partly_render_environment(&target->trees, 0, 2);
    render_shifting_texture(&target->ground);
    render_shifting_texture(&target->road);
    render_shifting_texture(&target->stripes);
    target->prev_point_count = target->point_count;
    if ((target->is_driving && (logic_layer.curr_tick - target->start_tick >= 2000)) || target->crash_tick != 0)
        render_traffic_on_pts(1, 9, target->car_ptr, &target->is_driving, &target->point_count);
    update_points(target);
    partly_render_environment(&target->trees, 3, ULONG_LONG_MAX);
    render_car(target->car_ptr);
    if ((target->is_driving && (logic_layer.curr_tick - target->start_tick >= 2000)) || target->crash_tick != 0)
        render_traffic_on_pts(10, UINT_MAX, NULL, NULL, NULL);
    render_texture(&target->personal_best_text);
    render_texture(&target->curr_points_text);
    return;
}


void update_points(struct Gameplay_Scene* target)
{
    int exit_code = EXIT_SUCCESS; /// A dummy for now.
    if (target->point_count == target->prev_point_count)
        return;
    
    char curr_pts[10];
    sprintf(curr_pts, "Pts: %d", target->point_count);
    free_texture(&target->curr_points_text);
    target->curr_points_text = create_text(curr_pts, (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(5, 20), 15, 1, &exit_code);

    if (target->point_count > PERSONAL_BEST)
        PERSONAL_BEST = target->point_count; /// Text will be updated in the next gameplay scene loading.
}