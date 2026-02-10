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


/* Variables */
struct Gameplay_Scene gameplay_scene = {0};


/* Predef */

void load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code);
void free_gameplay_scene(void);
void render_gameplay_scene(void);
void update_points(void);


/* Body */

void load_gameplay_scene(const char path[], struct Car* car_ptr, int* exit_code)
{
    /// Param checking
    if (exit_code == NULL)
        print_warning("`load_gameplay_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (path == NULL)
    {
        print_error("`load_gameplay_scene()`: `path` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    if (car_ptr == NULL)
    {
        print_error("`load_gameplay_scene()`: `car_ptr` arg is `NULL`", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    
    gameplay_scene.car_ptr = car_ptr;

    /// Reading the file
    char** scene_data = read_file_by_line(path, GAMEPLAY_DATA_LINES);
    if (scene_data == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't read data from file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }
    
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(7, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return;
    }

    /// Sky
    gameplay_scene.sky_bg = IMG_LoadTexture(graphics_layer.renderer, scene_data[0]);
    if (gameplay_scene.sky_bg == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`load_gameplay_scene()`: couldn't load the sky texture, replaced with null texture", IS_SDL_ERROR);
            gameplay_scene.sky_bg = NULL_TEXTURE;
        }
        else
        {
            print_error("`load_gameplay_scene()`: couldn't load the sky texture, and null texture is empty", IS_SDL_ERROR);
            free_deinit_stack(&deinit_stack);
            free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
            *exit_code = EXIT_FAILURE;
            return;
        }
    }
    if (gameplay_scene.sky_bg != NULL_TEXTURE)
        add_to_deinit_stack(&deinit_stack, gameplay_scene.sky_bg, (void (*)(void*))SDL_DestroyTexture);

    /// Ground
    gameplay_scene.ground = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 4, 150, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the ground texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return;
    }
    gameplay_scene.ground.freeze_shifting = true;
    add_to_shifting_texture(&gameplay_scene.ground, scene_data[1], exit_code); /// TODO: find a way
    add_to_shifting_texture(&gameplay_scene.ground, scene_data[2], exit_code); /// to check `exit_code`
    add_to_shifting_texture(&gameplay_scene.ground, scene_data[3], exit_code); /// w/o obnoxious
    add_to_shifting_texture(&gameplay_scene.ground, scene_data[4], exit_code); /// `if() {}` blocks.
    add_to_deinit_stack(&deinit_stack, &gameplay_scene.ground, (void (*)(void*))free_shifting_texture);

    /// Road
    gameplay_scene.road = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 3, 100, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the road texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return;
    }
    gameplay_scene.road.freeze_shifting = true;
    add_to_shifting_texture(&gameplay_scene.road, scene_data[5], exit_code);
    add_to_shifting_texture(&gameplay_scene.road, scene_data[6], exit_code);
    add_to_shifting_texture(&gameplay_scene.road, scene_data[7], exit_code);
    add_to_deinit_stack(&deinit_stack, &gameplay_scene.road, (void (*)(void*))free_shifting_texture);
    
    /// Stripes
    gameplay_scene.stripes = init_shifting_texture((SDL_FRect){0, RENDER_HEIGHT - 100, 240, 100}, 3, 100, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the stripes texture", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return;
    }
    gameplay_scene.stripes.freeze_shifting = true;
    add_to_shifting_texture(&gameplay_scene.stripes, scene_data[8], exit_code);
    add_to_shifting_texture(&gameplay_scene.stripes, scene_data[9], exit_code);
    add_to_shifting_texture(&gameplay_scene.stripes, scene_data[10], exit_code);
    add_to_deinit_stack(&deinit_stack, &gameplay_scene.stripes, (void (*)(void*))free_shifting_texture);

    /// Trees
    gameplay_scene.trees = new_environment((char*[]){scene_data[11], scene_data[12], scene_data[13], scene_data[14]}, 4, 7, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't load the trees", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return;
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
    couple_move_component_to_environment(&gameplay_scene.trees, tree_move_component, vec2(10,0), exit_code); /// TODO: exit code check.
    add_to_deinit_stack(&deinit_stack, &gameplay_scene.trees, (void (*)(void*))free_environment); /// Don't delete. More elements will be added later

    /// TODO: checks.
    gameplay_scene.clouds[0] = load_texture(scene_data[15], (SDL_FRect){(float)randint(0, RENDER_WIDTH-30),  5, 30, 10}, exit_code);
    gameplay_scene.clouds[1] = load_texture(scene_data[16], (SDL_FRect){(float)randint(0, RENDER_WIDTH-32), 12, 32, 15}, exit_code);
    gameplay_scene.clouds[2] = load_texture(scene_data[17], (SDL_FRect){(float)randint(0, RENDER_WIDTH-23), 24, 23,  8}, exit_code);
    gameplay_scene.clouds[3] = load_texture(scene_data[18], (SDL_FRect){(float)randint(0, RENDER_WIDTH-41), 29, 41,  8}, exit_code);
    gameplay_scene.clouds[4] = load_texture(scene_data[19], (SDL_FRect){(float)randint(0, RENDER_WIDTH-36), 34, 36,  8}, exit_code);
    gameplay_scene.clouds[5] = load_texture(scene_data[15], (SDL_FRect){(float)randint(0, RENDER_WIDTH-30),  5, 30, 10}, exit_code);
    gameplay_scene.clouds[6] = load_texture(scene_data[16], (SDL_FRect){(float)randint(0, RENDER_WIDTH-32), 12, 32, 15}, exit_code);
    gameplay_scene.clouds[7] = load_texture(scene_data[17], (SDL_FRect){(float)randint(0, RENDER_WIDTH-23), 24, 23,  8}, exit_code);
    gameplay_scene.clouds[8] = load_texture(scene_data[18], (SDL_FRect){(float)randint(0, RENDER_WIDTH-41), 29, 41,  8}, exit_code);
    gameplay_scene.clouds[9] = load_texture(scene_data[19], (SDL_FRect){(float)randint(0, RENDER_WIDTH-36), 34, 36,  8}, exit_code);

    /// Pause screen
    gameplay_scene.pause_screen = init_pause_screen(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't create pause screen", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return;
    }
    add_to_deinit_stack(&deinit_stack, &gameplay_scene.pause_screen, (void (*)(void*))free_pause_screen);
    /// Below block: DON'T MOVE out of this function!!! Some memory fuckery will happen.
    /// - Pause screen: setting button neighbours.
    ///                    | Target                                     | Up                                      | Down                                       |Left |Right
    add_neighbors_to_button(&gameplay_scene.pause_screen.          close_button, NULL,                                     &gameplay_scene.pause_screen.       continue_button, NULL, NULL);
    add_neighbors_to_button(&gameplay_scene.pause_screen.       continue_button, &gameplay_scene.pause_screen.       close_button, &gameplay_scene.pause_screen.   quit_to_menu_button, NULL, NULL);
    add_neighbors_to_button(&gameplay_scene.pause_screen.   quit_to_menu_button, &gameplay_scene.pause_screen.    continue_button, &gameplay_scene.pause_screen.quit_to_desktop_button, NULL, NULL);
    add_neighbors_to_button(&gameplay_scene.pause_screen.quit_to_desktop_button, &gameplay_scene.pause_screen.quit_to_menu_button, NULL,                                        NULL, NULL);
    gameplay_scene.pause_screen.curr_button = &gameplay_scene.pause_screen.continue_button;

    init_traffic_manager(5, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_gameplay_scene()`: couldn't initialize traffic manager", NON_SDL_ERROR);
        free_traffic_manager();
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
        return;
    }
    
    char personal_best_text[10];
    sprintf(personal_best_text, "PB: %d", PERSONAL_BEST);
    gameplay_scene.personal_best_text = create_text(personal_best_text, (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(5, 5), 15, 1, exit_code);
    add_to_deinit_stack(&deinit_stack, &gameplay_scene.personal_best_text, (void (*)(void*))free_texture);

    gameplay_scene.curr_points_text = create_text("Pts: 0", (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(5, 20), 15, 1, exit_code);
    
    free_deinit_stack(&deinit_stack); /// `free` because those resources will be used.
    free_ptr_arr((void**)scene_data, GAMEPLAY_DATA_LINES);
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_gameplay_scene(void)
{
    if (gameplay_scene.car_ptr != NULL)
    {
        gameplay_scene.car_ptr->coords.x     = center_x(gameplay_scene.car_ptr->coords.w);
        gameplay_scene.car_ptr->base_texture = 2;
        gameplay_scene.car_ptr->latest_turn_start     = 0;
        gameplay_scene.car_ptr->latest_turn_end       = 0;
        gameplay_scene.car_ptr->prev_turn_direction_x = 0;
    }
    
    free_texture(&gameplay_scene.personal_best_text);
    free_pause_screen(&gameplay_scene.pause_screen);
    for (size_t i = 0; i < 10; i++)
        free_texture(&gameplay_scene.clouds[i]);
    free_environment     (&gameplay_scene.trees);
    free_shifting_texture(&gameplay_scene.stripes);
    free_shifting_texture(&gameplay_scene.road);
    free_shifting_texture(&gameplay_scene.ground);
    if (gameplay_scene.sky_bg != NULL && gameplay_scene.sky_bg != NULL_TEXTURE)
    {
        SDL_DestroyTexture(gameplay_scene.sky_bg);
        gameplay_scene.sky_bg = NULL;
    }

    memset(&gameplay_scene, 0, sizeof gameplay_scene);
    return;
}


void render_gameplay_scene(void)
{
    /// TODO: check `gameplay_scene` members.

    if (gameplay_scene.pause_screen.is_open)
    {
        render_pause_screen(&gameplay_scene.pause_screen);
        return;
    }
    
    /// Moving stuff
    if (gameplay_scene.is_driving)
    {
        move_all_rects(gameplay_scene.trees.move_component);
        gameplay_scene.ground. freeze_shifting = false;
        gameplay_scene.road.   freeze_shifting = false;
        gameplay_scene.stripes.freeze_shifting = false;
        if ((gameplay_scene.   start_tick != 0)
         && (logic_layer.curr_tick - gameplay_scene.start_tick >= 2000)) /// Pause before traffic starts coming.
            move_traffic(MOVE_NORMAL);
    }
    else if (! gameplay_scene.is_driving)
    {
        gameplay_scene.ground. freeze_shifting = true;
        gameplay_scene.road.   freeze_shifting = true;
        gameplay_scene.stripes.freeze_shifting = true;
        if (gameplay_scene.start_tick != 0) /// Checks that we're not on the start.
        {
            move_traffic(MOVE_REVERSE);
            if (gameplay_scene.crash_tick == 0)
                gameplay_scene.crash_tick = logic_layer.curr_tick;
            else if (gameplay_scene.crash_tick != ULONG_LONG_MAX && logic_layer.curr_tick - gameplay_scene.crash_tick >= 2000)
            {
                gameplay_scene.crash_tick = ULONG_LONG_MAX; /// So that the pause menu opens exactly once.
                show_pause_screen(&gameplay_scene.pause_screen);
            }
        }
    }

    /// Rendering
    SDL_RenderTexture(graphics_layer.renderer, gameplay_scene.sky_bg, NULL, NULL);
    for (size_t i = 0; i < 10; i++)
    {
        /// Just a bunch of magic numbers I got after playing around.
        gameplay_scene.clouds[i].rect.x += (float)CLOUD_VELOCITY * (float)FPS_manager.delta_ns / SEC_IN_NS / gameplay_scene.clouds[i].rect.w;
        if (gameplay_scene.clouds[i].rect.x >= RENDER_WIDTH - 1)
            gameplay_scene.clouds[i].rect.x = - gameplay_scene.clouds[i].rect.w + 1;
        render_texture(&gameplay_scene.clouds[i]);
    }
    if ((gameplay_scene.is_driving && (logic_layer.curr_tick - gameplay_scene.start_tick >= 2000)) || gameplay_scene.crash_tick != 0)
        render_traffic_on_pts(0, 0, NULL, NULL, NULL);
    partly_render_environment(&gameplay_scene.trees, 0, 2);
    render_shifting_texture(&gameplay_scene.ground);
    render_shifting_texture(&gameplay_scene.road);
    render_shifting_texture(&gameplay_scene.stripes);
    gameplay_scene.prev_point_count = gameplay_scene.point_count;
    if ((gameplay_scene.is_driving && (logic_layer.curr_tick - gameplay_scene.start_tick >= 2000)) || gameplay_scene.crash_tick != 0)
        render_traffic_on_pts(1, 9, gameplay_scene.car_ptr, &gameplay_scene.is_driving, &gameplay_scene.point_count);
    update_points();
    partly_render_environment(&gameplay_scene.trees, 3, ULONG_LONG_MAX);
    render_car(gameplay_scene.car_ptr);
    if ((gameplay_scene.is_driving && (logic_layer.curr_tick - gameplay_scene.start_tick >= 2000)) || gameplay_scene.crash_tick != 0)
        render_traffic_on_pts(10, UINT_MAX, NULL, NULL, NULL);
    render_texture(&gameplay_scene.personal_best_text);
    render_texture(&gameplay_scene.curr_points_text);
    return;
}


void update_points(void)
{
    int exit_code = EXIT_SUCCESS; /// A dummy for now.
    if (gameplay_scene.point_count == gameplay_scene.prev_point_count)
        return;
    
    char curr_pts[10];
    sprintf(curr_pts, "Pts: %d", gameplay_scene.point_count);
    free_texture(&gameplay_scene.curr_points_text);
    gameplay_scene.curr_points_text = create_text(curr_pts, (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(5, 20), 15, 1, &exit_code);

    if (gameplay_scene.point_count > PERSONAL_BEST)
        PERSONAL_BEST = gameplay_scene.point_count; /// Text will be updated in the next gameplay scene loading.
}