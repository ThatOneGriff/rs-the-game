#pragma once
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "../_miniaudio/miniaudio.h"


struct Audio_Manager
{
    ma_engine engine;
    ma_sound  music ;
    bool using_audio;
};
static struct Audio_Manager audio_manager; /// Singleton.

#endif /// AUDIO_MANAGER_H