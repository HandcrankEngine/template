// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <memory>

#include "SDL_mixer.h"

#include "Utilities.hpp"

namespace Handcrank
{

namespace
{
std::unordered_map<std::string, std::shared_ptr<Mix_Music>> audioMusicCache =
    std::unordered_map<std::string, std::shared_ptr<Mix_Music>>();
std::unordered_map<std::string, std::shared_ptr<Mix_Chunk>> audioSFXCache =
    std::unordered_map<std::string, std::shared_ptr<Mix_Chunk>>();
} // namespace

struct MusicDeleter
{
    void operator()(Mix_Music *music) const
    {
        if (music != nullptr)
        {
            Mix_FreeMusic(music);
        }
    }
};

struct SFXDeleter
{
    void operator()(Mix_Chunk *sfx) const
    {
        if (sfx != nullptr)
        {
            Mix_FreeChunk(sfx);
        }
    }
};

inline auto SetupAudio() -> int
{
    return Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

inline auto LoadMusic(const char *path) -> std::shared_ptr<Mix_Music>
{
    if (audioMusicCache.find(path) != audioMusicCache.end())
    {
        return audioMusicCache.find(path)->second;
    }

    auto *music = Mix_LoadMUS(path);

    if (music == nullptr)
    {
        return nullptr;
    }

    auto ptr = std::shared_ptr<Mix_Music>(music, MusicDeleter());

    audioMusicCache.insert_or_assign(path, ptr);

    return ptr;
}

inline auto LoadMusic(const void *mem, const int size)
    -> std::shared_ptr<Mix_Music>
{

    auto hash = MemHash(mem, size);

    if (audioMusicCache.find(hash) != audioMusicCache.end())
    {
        return audioMusicCache.find(hash)->second;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    if (rw == nullptr)
    {
        return nullptr;
    }

    auto *music = Mix_LoadMUS_RW(rw, 1);

    if (music == nullptr)
    {
        return nullptr;
    }

    auto ptr = std::shared_ptr<Mix_Music>(music, MusicDeleter());

    audioMusicCache.insert_or_assign(hash, ptr);

    return ptr;
}

inline auto PlayMusic(const std::shared_ptr<Mix_Music> &music) -> int
{
    if (music == nullptr)
    {
        return -1;
    }

    return Mix_PlayMusic(music.get(), -1);
}

inline auto LoadSFX(const char *path) -> std::shared_ptr<Mix_Chunk>
{
    if (audioSFXCache.find(path) != audioSFXCache.end())
    {
        return audioSFXCache.find(path)->second;
    }

    auto *sfx = Mix_LoadWAV(path);

    if (sfx == nullptr)
    {
        return nullptr;
    }

    auto ptr = std::shared_ptr<Mix_Chunk>(sfx, SFXDeleter());

    audioSFXCache.insert_or_assign(path, ptr);

    return ptr;
}

inline auto LoadSFX(const void *mem, const int size)
    -> std::shared_ptr<Mix_Chunk>
{
    auto hash = MemHash(mem, size);

    if (audioSFXCache.find(hash) != audioSFXCache.end())
    {
        return audioSFXCache.find(hash)->second;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    if (rw == nullptr)
    {
        return nullptr;
    }

    auto *sfx = Mix_LoadWAV_RW(rw, 1);

    if (sfx == nullptr)
    {
        return nullptr;
    }

    auto ptr = std::shared_ptr<Mix_Chunk>(sfx, SFXDeleter());

    audioSFXCache.insert_or_assign(hash, ptr);

    return ptr;
}

inline auto PlaySFX(const std::shared_ptr<Mix_Chunk> &sfx) -> int
{
    if (sfx == nullptr)
    {
        return -1;
    }

    return Mix_PlayChannel(-1, sfx.get(), 0);
}

} // namespace Handcrank
