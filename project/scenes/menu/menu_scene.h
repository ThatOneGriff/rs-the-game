#pragma once
#ifndef MENU_SCENE_H
#define MENU_SCENE_H

/* SDL3 */
#include <SDL3/SDL.h> /// SDL stuff.

/* Graphics and components */
#include "../car.h"                          /// Car data.
#include "options_screen.h"                  /// Options screen.
#include "../../game_components/ui/button.h"          /// Button.
#include "../../game_components/graphics/texture.h"   /// Texture.

#define MENU_DATA_PATH "./rsdt/scene_data/menu.rsdt"
#define MENU_DATA_LINES 1


/* Struct */

struct Menu_Scene
{
    struct Car* car_ptr;

    SDL_Texture* bg;
    struct Texture car_name_text;

    struct Button  prev_button;
    struct Button  next_button;
    struct Button  play_button;
    struct Button  options_button;
    struct Button  quit_button;
    struct Button* curr_button;
    struct Options_Screen options_screen;

    struct Texture photo_quad1;
    struct Texture photo_quad2;
    struct Texture photo_quad3;
    struct Texture photo_quad4;
    
    struct Texture year_text;
    struct Texture horsepower_text;
    struct Texture top_speed_text;
    struct Texture handling_text;
    
    struct Texture info_line1;
    struct Texture info_line2;
};
extern struct Menu_Scene menu_scene; /// Singleton.


/* Predef */

void load_menu_scene  (struct Car *const car_ptr, int *const exit_code);
void free_menu_scene  (void);
void set_menu_car_info(struct Car *const car_ptr, int *const exit_code);
void render_menu_scene(void);


#endif /// MENU_SCENE_H