// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <unordered_map>

#include <SDL_mixer.h>

namespace HandcrankEngine
{

inline const int DEFAULT_AUDIO_CHUNK_SIZE = 4096;

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

inline auto TeardownAudio() -> void
{
    if (audioIsOpen)
    {
        Mix_CloseAudio();

        audioIsOpen = false;
    }
}

} // namespace HandcrankEngine
