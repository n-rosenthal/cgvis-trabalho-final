#pragma once

#include "audio/miniaudio.h"
#include <string>

class SoundManager
{
public:

    SoundManager();
    ~SoundManager();

    bool init();

    void play(const std::string& filename);

private:

    ma_engine engine;
};