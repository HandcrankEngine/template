// Handcrank Engine - https://handcrankengine.com/
//
// ░█░█░█▀█░█▀█░█▀▄░█▀▀░█▀▄░█▀█░█▀█░█░█░░░█▀▀░█▀█░█▀▀░▀█▀░█▀█░█▀▀
// ░█▀█░█▀█░█░█░█░█░█░░░█▀▄░█▀█░█░█░█▀▄░░░█▀▀░█░█░█░█░░█░░█░█░█▀▀
// ░▀░▀░▀░▀░▀░▀░▀▀░░▀▀▀░▀░▀░▀░▀░▀░▀░▀░▀░░░▀▀▀░▀░▀░▀▀▀░▀▀▀░▀░▀░▀▀▀
//
// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <unordered_map>

#include <SDL_mixer.h>

#include "Utilities.hpp"

namespace HandcrankEngine
{

inline const int DEFAULT_AUDIO_CHUNK_SIZE = 512;

namespace
{
bool audioIsOpen = false;

inline std::unordered_map<std::size_t, std::shared_ptr<Mix_Music>>
    audioMusicCache =
        std::unordered_map<std::size_t, std::shared_ptr<Mix_Music>>();
inline std::unordered_map<std::size_t, std::shared_ptr<Mix_Chunk>>
    audioSFXCache =
        std::unordered_map<std::size_t, std::shared_ptr<Mix_Chunk>>();
} // namespace

inline auto ClearAudioCache() -> void
{
    audioMusicCache.clear();
    audioSFXCache.clear();
}

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

inline auto SetupAudio() -> int
{
    if (audioIsOpen)
    {
        return 0;
    }

    auto result = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
                                MIX_DEFAULT_CHANNELS, DEFAULT_AUDIO_CHUNK_SIZE);

    if (result == 0)
    {
        audioIsOpen = true;
    }

    return result;
}

inline auto LoadCachedMusic(const char *path) -> std::shared_ptr<Mix_Music>
{
    auto cacheKey = std::hash<std::string_view>{}(std::string_view(path));

    auto match = audioMusicCache.find(cacheKey);

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

    audioMusicCache.insert_or_assign(cacheKey, music);

    return music;
}

inline auto LoadCachedMusic(const void *mem, int size)
    -> std::shared_ptr<Mix_Music>
{
    auto cacheKey = MemHash(mem, size);

    auto match = audioMusicCache.find(cacheKey);

    if (match != audioMusicCache.end())
    {
        return match->second;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    if (rw == nullptr)
    {
        return nullptr;
    }

    if (SetupAudio() != 0)
    {
        return nullptr;
    }

    auto music =
        std::shared_ptr<Mix_Music>(Mix_LoadMUS_RW(rw, 1), MixMusicDeleter{});

    if (music == nullptr)
    {
        return nullptr;
    }

    audioMusicCache.insert_or_assign(cacheKey, music);

    return music;
}

inline auto LoadCachedSFX(const char *path) -> std::shared_ptr<Mix_Chunk>
{
    auto cacheKey = std::hash<std::string_view>{}(std::string_view(path));

    auto match = audioSFXCache.find(cacheKey);

    if (match != audioSFXCache.end())
    {
        return match->second;
    }

    if (SetupAudio() != 0)
    {
        return nullptr;
    }

    auto sfx = std::shared_ptr<Mix_Chunk>(Mix_LoadWAV(path), MixChunkDeleter{});

    if (sfx == nullptr)
    {
        return nullptr;
    }

    audioSFXCache.insert_or_assign(cacheKey, sfx);

    return sfx;
}

inline auto LoadCachedSFX(const void *mem, int size)
    -> std::shared_ptr<Mix_Chunk>
{
    auto cacheKey = MemHash(mem, size);

    auto match = audioSFXCache.find(cacheKey);

    if (match != audioSFXCache.end())
    {
        return match->second;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    if (rw == nullptr)
    {
        return nullptr;
    }

    if (SetupAudio() != 0)
    {
        return nullptr;
    }

    auto sfx =
        std::shared_ptr<Mix_Chunk>(Mix_LoadWAV_RW(rw, 1), MixChunkDeleter{});

    if (sfx == nullptr)
    {
        return nullptr;
    }

    audioSFXCache.insert_or_assign(cacheKey, sfx);

    return sfx;
}

inline auto TeardownAudio() -> void
{
    if (audioIsOpen)
    {
        Mix_CloseAudio();

        audioIsOpen = false;
    }
}

} // namespace HandcrankEngine
