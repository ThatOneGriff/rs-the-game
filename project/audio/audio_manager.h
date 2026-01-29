#pragma once
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "../_miniaudio/miniaudio.h"

/// For now, it's miniature enough to not be mitosed.

/// As element count adds up, this
/// may get its own `init` and `quit` functions.

struct Audio_Manager
{
    ma_engine engine;
    ma_sound  music ;
    bool using_audio;
};
static struct Audio_Manager audio_manager; /// Singleton.

#endif /// AUDIO_MANAGER_H