#include "audio/AudioManager.hpp"

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
    ma_engine_uninit(&engine);
}

bool SoundManager::init()
{
    return ma_engine_init(NULL, &engine) == MA_SUCCESS;
}

void SoundManager::play(const std::string& filename)
{
    ma_engine_play_sound(
        &engine,
        filename.c_str(),
        NULL
    );
}