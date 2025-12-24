#pragma once
#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include <SDL3/SDL.h> /// SDL3.
#include <SDL3_image/SDL_image.h> /// SDL3_image.

#include <stdlib.h> /// `malloc()`

#include "menu_controls.h" /// Controls.
#include "../../debug.h"   /// Error printing.
#include "../../helpers/helpers.h" /// Colors.
#include "../../graphics/graphics_layer.h" /// `graphics_layer`.
#include "../../text/border.h" /// Selected text highlighting.
#include "../../text/text.h"   /// Text.


/* Struct */

struct Menu_Scene
{
    SDL_Texture* bg;
    
    struct Texture car_name_text;
    /*struct Texture main_car_picture;
    struct Texture extra_car_pictures[4];

    struct Texture FPS_text;
    struct Texture FPS_value_text;

    struct Texture play_text;

    void* selectable;
    size_t selected;*/
};
struct Menu_Scene load_menu_scene(int* exit_code);
void              free_menu_scene(struct Menu_Scene* target);
void            render_menu_scene(struct Menu_Scene* target, int* exit_code);


/* Functions */

/// TEMP: very rigid. I didn't have time.
struct Menu_Scene load_menu_scene(int* exit_code)
{
    struct Menu_Scene result;
    result.bg = IMG_LoadTexture(graphics_layer.renderer, "res/images/menu/bg.png");
    result.car_name_text = create_text("Renault Clio Williams", SDL_WHITE, SDL_WHITE, 20, 0, exit_code);
    //result.main_car_picture = load_texture("")
    *exit_code = EXIT_SUCCESS;
    return result;
}


void render_menu_scene(struct Menu_Scene* target, int* exit_code)
{
    SDL_RenderTexture(graphics_layer.renderer, target->bg, NULL, NULL);
    render_texture(&target->car_name_text);
    *exit_code = EXIT_SUCCESS;
    return;
}

#endif /// MENU_SCENE_H