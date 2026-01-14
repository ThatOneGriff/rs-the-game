#pragma once
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "miniaudio.h"


struct Audio_Manager
{
    ma_engine engine;
    ma_sound  track;
};
static struct Audio_Manager audio_manager; /// Singleton.

#endif /// AUDIO_MANAGER_H