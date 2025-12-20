#pragma once
#ifndef LOGIC_LAYER_H
#define LOGIC_LAYER_H


/* Predef */

struct Logic_Layer;
void _init_logic_layer(void);
static struct Logic_Layer logic_layer; /// Singleton.


/* Body */

struct Logic_Layer
{
    SDL_Event event;
    //bool* key_state;
    bool  game_is_running;
};
void _init_logic_layer(void)
{
    logic_layer.game_is_running = true;
}

#endif /// LOGIC_LAYER_H