#pragma once
#ifndef GAMEPLAY_SCENE_H
#define GAMEPLAY_SCENE_H

#include <SDL3/SDL.h>       /// SDL3
#include <SDL3_image/SDL.h> /// SDL3_image

#include "../../debug.h" /// Error printing.
#include "gameplay_controls.h" /// Controls.
#include "car.h"               /// Car (player).

#include "../../game_components/multi_texture.h" /// Textures
#include "../../game_components/texture.h"       /// Multi-textures


/* Struct */

struct Gameplay_Scene
{
    SDL_Texture* sky_bg; /// Not a `Texture`, because it's rendered on the whole screen.
    struct Texture ground;
    struct Multi_Texture trees;
};
struct Gameplay_Scene load_gameplay_scene(const char* sky_bg_path, const char* ground_path, const char* tree_path);

#endif /// GAMEPLAY_SCENE_H