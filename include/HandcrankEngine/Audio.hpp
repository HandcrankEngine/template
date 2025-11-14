// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL_mixer.h>

#include "AudioCache.hpp"
#include "Utilities.hpp"

namespace HandcrankEngine
{

struct MixMusicDeleter
{
    void operator()(Mix_Music *music) const
    {
        if (music != nullptr)
        {
            Mix_FreeMusic(music);
        }
    }
};

struct MixChunkDeleter
{
    void operator()(Mix_Chunk *chunk) const
    {
        if (chunk != nullptr)
        {
            Mix_FreeChunk(chunk);
        }
    }
};

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

inline auto LoadMusic(const char *path) -> std::shared_ptr<Mix_Music>
{
    auto match = audioMusicCache.find(path);

    if (match != audioMusicCache.end())
    {
        return match->second;
    }

    if (SetupAudio() != 0)
    {
        return nullptr;
    }

    auto music =
        std::shared_ptr<Mix_Music>(Mix_LoadMUS(path), MixMusicDeleter{});

    if (music == nullptr)
    {
        return nullptr;
    }

    audioMusicCache.insert_or_assign(path, music);

    return music;
}

inline auto LoadMusic(const void *mem, const int size)
    -> std::shared_ptr<Mix_Music>
{
    auto hash = MemHash(mem, size);

    auto match = audioMusicCache.find(hash);

    if (match != audioMusicCache.end())
    {
        return match->second;
    }

    if (SetupAudio() != 0)
    {
        return nullptr;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    if (rw == nullptr)
    {
        return nullptr;
    }

    auto music =
        std::shared_ptr<Mix_Music>(Mix_LoadMUS_RW(rw, 1), MixMusicDeleter{});

    if (music == nullptr)
    {
        return nullptr;
    }

    audioMusicCache.insert_or_assign(hash, music);

    return music;
}

inline auto LoadSFX(const char *path) -> std::shared_ptr<Mix_Chunk>
{
    auto match = audioSFXCache.find(path);

    if (match != audioSFXCache.end())
    {
        return match->second;
    }

    auto sfx = std::shared_ptr<Mix_Chunk>(Mix_LoadWAV(path), MixChunkDeleter{});

    if (sfx == nullptr)
    {
        return nullptr;
    }

    audioSFXCache.insert_or_assign(path, sfx);

    return sfx;
}

inline auto LoadSFX(const void *mem, const int size)
    -> std::shared_ptr<Mix_Chunk>
{
    auto hash = MemHash(mem, size);

    auto match = audioSFXCache.find(hash);

    if (match != audioSFXCache.end())
    {
        return match->second;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    if (rw == nullptr)
    {
        return nullptr;
    }

    auto sfx =
        std::shared_ptr<Mix_Chunk>(Mix_LoadWAV_RW(rw, 1), MixChunkDeleter{});

    if (sfx == nullptr)
    {
        return nullptr;
    }

    audioSFXCache.insert_or_assign(hash, sfx);

    return sfx;
}

} // namespace HandcrankEngine
