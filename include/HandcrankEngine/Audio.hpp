// Handcrank Engine - https://handcrankengine.com/
//
// ░█░█░█▀█░█▀█░█▀▄░█▀▀░█▀▄░█▀█░█▀█░█░█░░░█▀▀░█▀█░█▀▀░▀█▀░█▀█░█▀▀
// ░█▀█░█▀█░█░█░█░█░█░░░█▀▄░█▀█░█░█░█▀▄░░░█▀▀░█░█░█░█░░█░░█░█░█▀▀
// ░▀░▀░▀░▀░▀░▀░▀▀░░▀▀▀░▀░▀░▀░▀░▀░▀░▀░▀░░░▀▀▀░▀░▀░▀▀▀░▀▀▀░▀░▀░▀▀▀
//
// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL_mixer.h>

namespace HandcrankEngine
{

inline auto PlayMusic(Mix_Music *music) -> int
{
    if (music == nullptr)
    {
        return -1;
    }

    return Mix_PlayMusic(music, -1);
}

inline auto PlaySFX(Mix_Chunk *sfx) -> int
{
    if (sfx == nullptr)
    {
        return -1;
    }

    return Mix_PlayChannel(-1, sfx, 0);
}

inline auto PlaySFXOnChannel(int channel, Mix_Chunk *sfx) -> int
{
    if (sfx == nullptr)
    {
        return -1;
    }

    return Mix_PlayChannel(channel, sfx, 0);
}

inline void StopAllMusic() { Mix_HaltMusic(); }

inline void StopAllSFX() { Mix_HaltChannel(-1); }

} // namespace HandcrankEngine
