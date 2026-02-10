/* Related header */
#include "menu_scene.h"

/* SDL3 */
#include <SDL3/SDL.h>             /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

/* C headers */
#include <stdlib.h>  /// `malloc()`
#include <stdbool.h> /// Setting button focuses.
#include <string.h> /// `memset()`.

/* Helper headers */
#include "../../debug.h"           /// Error printing.
#include "../../deinit_stack.h"    /// Deinitialization stack.
#include "../../helpers/helpers.h" /// `free_ptr_arr()`.
#include "../../resources.h"       /// Null texture.

/* Graphics and components */
#include "../car.h"                          /// Car data.
#include "../car_manager.h"                  /// Minor car manager data pulling for 'Prev'/'Next' button rendering.
#include "options_screen.h"                  /// Options screen.
#include "../../game_components/ui/button.h" /// Button.
#include "../../game_components/graphics/texture.h" /// Texture.
#include "../../game_components/text/text.h" /// Text creation.
#include "../../graphics/graphics_layer.h"   /// `graphics_layer`.


/* Variables */
struct Menu_Scene menu_scene = {0};


/* Predef */

void load_menu_scene(struct Car* car_ptr, int* exit_code);
void free_menu_scene(void);
void set_menu_car_info(struct Car* car, int* exit_code);
void render_menu_scene(void);


/* Body */

void load_menu_scene(struct Car* car_ptr, int* exit_code)
{
    /// Param checking
    if (exit_code == NULL)
        print_warning("`load_menu_scene()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    
    /// Reading the file
    char** scene_data = read_file_by_line(MENU_DATA_PATH, MENU_DATA_LINES);
    if (scene_data == NULL)
    {
        print_error("`load_gameplay_scene()`: couldn't read data from file", NON_SDL_ERROR);
        *exit_code = EXIT_FAILURE;
        return;
    }

    /// Background
    menu_scene.bg = IMG_LoadTexture(graphics_layer.renderer, scene_data[0]);
    if (menu_scene.bg == NULL)
    {
        if (NULL_TEXTURE != NULL)
        {
            print_warning("`load_menu_scene()`: couldn't load the bg texture, replaced by null texture", IS_SDL_ERROR);
            menu_scene.bg = NULL_TEXTURE;
        }
        else
        {
            print_error("`load_menu_scene()`: couldn't load the bg texture, and null texture is empty", IS_SDL_ERROR);
            free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
            *exit_code = EXIT_FAILURE;
            return;
        }
    }
    /// Deinit stack
    struct Deinit_Stack deinit_stack = new_deinit_stack(6, exit_code); /// Not adding the last element (font loading) or those that need their own function treatment.
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`init()`: couldn't instance a deinitialization stack", NON_SDL_ERROR);
        free_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return;
    }
    if (menu_scene.bg != NULL_TEXTURE)
        add_to_deinit_stack(&deinit_stack, menu_scene.bg, (void (*)(void*))SDL_DestroyTexture);

    /// Prev button
    menu_scene.prev_button = create_button("PREV", (SDL_Color){69,71,75,255}, vec2(155, 15), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create prev button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return;
    }
    add_to_deinit_stack(&deinit_stack, &menu_scene.prev_button, (void (*)(void*))free_button);

    /// Next button
    menu_scene.next_button = create_button("NEXT", (SDL_Color){69,71,75,255}, vec2(195, 15), 12, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create next button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return;
    }
    add_to_deinit_stack(&deinit_stack, &menu_scene.next_button, (void (*)(void*))free_button);

    /// Play button
    menu_scene.play_button = create_button("PLAY", (SDL_Color){254,178,26,255}, vec2(155, 82), 25, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create play button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return;
    }
    menu_scene.play_button.is_focused = true;
    add_to_deinit_stack(&deinit_stack, &menu_scene.play_button, (void (*)(void*))free_button);

    /// Options button
    menu_scene.options_button = create_button("OPTIONS", (SDL_Color){19,70,134,255}, vec2(155, 107), 18, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create options button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return;
    }
    add_to_deinit_stack(&deinit_stack, &menu_scene.options_button, (void (*)(void*))free_button);

    /// Options screen
    menu_scene.options_screen = init_options_screen(exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't init options screen", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return;
    }
    add_to_deinit_stack(&deinit_stack, &menu_scene.options_screen, (void (*)(void*))free_options_screen);

    /// Quit button
    menu_scene.quit_button = create_button("QUIT", (SDL_Color){237,63,39,255}, vec2(155, 125), 18, 2, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`load_menu_scene()`: couldn't create quit button", NON_SDL_ERROR);
        flush_deinit_stack(&deinit_stack);
        free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
        return;
    }

    /// Button neighbors   | TARGET                    | UP                        | DOWN                      | LEFT                   | RIGHT
    add_neighbors_to_button(&menu_scene.prev_button,    NULL,                       &menu_scene.play_button,    NULL,                    &menu_scene.next_button);
    add_neighbors_to_button(&menu_scene.next_button,    NULL,                       &menu_scene.play_button,    &menu_scene.prev_button, NULL);
    add_neighbors_to_button(&menu_scene.play_button,    &menu_scene.prev_button,    &menu_scene.options_button, NULL,                    NULL);
    add_neighbors_to_button(&menu_scene.options_button, &menu_scene.play_button,    &menu_scene.quit_button,    NULL,                    NULL);
    add_neighbors_to_button(&menu_scene.quit_button,    &menu_scene.options_button, NULL,                       NULL,                    NULL);
    menu_scene.curr_button = &menu_scene.play_button;

    set_menu_car_info(car_ptr, exit_code);

    free_deinit_stack(&deinit_stack);
    free_ptr_arr((void**)scene_data, MENU_DATA_LINES);
    *exit_code = EXIT_SUCCESS;
    return;
}


void free_menu_scene(void)
{
    if (menu_scene.bg != NULL_TEXTURE)
        SDL_DestroyTexture(menu_scene.bg);
    free_texture(&menu_scene.car_name_text);
    free_button(&menu_scene.prev_button);
    free_button(&menu_scene.next_button);

    free_button(&menu_scene.play_button);
    free_button(&menu_scene.options_button);
    free_button(&menu_scene.quit_button);

    free_texture(&menu_scene.photo_quad1);
    free_texture(&menu_scene.photo_quad2);
    free_texture(&menu_scene.photo_quad3);
    free_texture(&menu_scene.photo_quad4);
    
    free_texture(&menu_scene.year_text);
    free_texture(&menu_scene.horsepower_text);
    free_texture(&menu_scene.top_speed_text);
    free_texture(&menu_scene.handling_text);
    
    free_texture(&menu_scene.info_line1);
    free_texture(&menu_scene.info_line2);

    memset(&menu_scene, 0, sizeof menu_scene);
    return;
}


void set_menu_car_info(struct Car* car, int* exit_code)
{
    if (exit_code == NULL)
        print_warning("`set_menu_car_info()`: `exit_code` arg is `NULL`", NON_SDL_ERROR);
    if (car == NULL) /// TODO: check all members.
    {
        print_error("`set_menu_car_info()`: `car` arg is `NULL`", NON_SDL_ERROR);
        return;
    }

    menu_scene.car_ptr = car;
    /// Car name text
    menu_scene.car_name_text = create_text(car->name, (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,0}, vec2(10, 10), 15, 1, exit_code);
    if (*exit_code == EXIT_FAILURE)
    {
        print_error("`set_menu_car_info()`: couldn't create the text", NON_SDL_ERROR);
        return;
    }
    
    /// TODO: checks.
    menu_scene.photo_quad1 = load_texture(car->quad_paths[0], (SDL_FRect){10   , 34   , 72, 54}, exit_code);
    menu_scene.photo_quad2 = load_texture(car->quad_paths[1], (SDL_FRect){10+72, 34   , 72, 54}, exit_code);
    menu_scene.photo_quad3 = load_texture(car->quad_paths[2], (SDL_FRect){10   , 34+54, 72, 54}, exit_code);
    menu_scene.photo_quad4 = load_texture(car->quad_paths[3], (SDL_FRect){10+72, 34+54, 72, 54}, exit_code);
    
    char year_text[15];
    sprintf(year_text, "Year: %d", car->year);
    char engine_text[15];
    sprintf(engine_text, "Engine: %d hp", car->hp);
    char speed_text[15];
    sprintf(speed_text, "Speed: %d kph", car->top_speed);
    char handling_text[14];
    sprintf(handling_text, "Handling: %d", car->handling);
    menu_scene.year_text        = create_text(year_text,     (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 33), 9, 1, exit_code);
    menu_scene.horsepower_text  = create_text(engine_text,   (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 43), 9, 1, exit_code);
    menu_scene.top_speed_text   = create_text(speed_text,    (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 53), 9, 1, exit_code);
    menu_scene.handling_text    = create_text(handling_text, (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(156, 63), 9, 1, exit_code);
    
    menu_scene.info_line1 = create_text(car->info_text[0], (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(10, 150), 9, 1, exit_code);
    menu_scene.info_line2 = create_text(car->info_text[1], (SDL_Color){255,255,255,255}, (SDL_Color){0,0,0,255}, vec2(10, 160), 9, 1, exit_code);
    
    *exit_code = EXIT_SUCCESS;
    return;
}


void render_menu_scene(void)
{
    /// TODO: `menu_scene` check.

    if (menu_scene.options_screen.is_open)
    {
        render_options_screen(&menu_scene.options_screen);
        return;
    }

    SDL_RenderTexture(graphics_layer.renderer, menu_scene.bg, NULL, NULL);
    render_texture(&menu_scene.car_name_text);
    if (players_car_manager.cur_car != 0)
        render_button(&menu_scene.prev_button);
    if (players_car_manager.cur_car != players_car_manager.car_count - 1)
        render_button(&menu_scene.next_button);

    render_button(&menu_scene.play_button);
    render_button(&menu_scene.options_button);
    render_button(&menu_scene.quit_button);

    render_texture(&menu_scene.photo_quad1);
    render_texture(&menu_scene.photo_quad2);
    render_texture(&menu_scene.photo_quad3);
    render_texture(&menu_scene.photo_quad4);
    
    render_texture(&menu_scene.year_text);
    render_texture(&menu_scene.horsepower_text);
    render_texture(&menu_scene.top_speed_text);
    render_texture(&menu_scene.handling_text);
    
    render_texture(&menu_scene.info_line1);
    render_texture(&menu_scene.info_line2);
    return;
}