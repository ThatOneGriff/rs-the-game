#pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include "miniaudio.h"

struct Audio
{
    ma_engine engine;
    ma_sound  track;
};
static struct Audio audio;

#endif /// AUDIO_H