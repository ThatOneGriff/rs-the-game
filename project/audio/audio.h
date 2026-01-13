#pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include "miniaudio.h"

struct Audio
{
    ma_engine engine;
    /// ...
};
static struct Audio audio;
//extern struct Audio audio;

#endif /// AUDIO_H