// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include "Utilities.hpp"

namespace HandcrankEngine
{

namespace
{
bool fontLoadedForFirstTime = false;

inline std::unordered_map<std::string, TTF_Font *> fontCache =
    std::unordered_map<std::string, TTF_Font *>();
} // namespace

inline auto ClearFontCache() -> void
{
    for (const auto &font : fontCache)
    {
        if (font.second != nullptr)
        {
            TTF_CloseFont(font.second);
        }
    }

    fontCache.clear();
}

inline auto CleanupFontInits() -> void
{
    for (auto i = 0; i < TTF_WasInit(); i += 1)
    {
        TTF_Quit();
    }
}

/**
 * Load font from a path.
 *
 * @param path File path to font file.
 * @param ptSize The size of the font.
 */
inline auto LoadCachedFont(const char *path, const int ptSize = 24)
    -> TTF_Font *
{
    auto cacheKey = std::string(path) + "_" + std::to_string(ptSize);

    auto match = fontCache.find(cacheKey);

    if (match != fontCache.end())
    {
        return match->second;
    }

    if (!fontLoadedForFirstTime)
    {
        if (TTF_WasInit() == 0)
        {
            TTF_Init();
        }

        fontLoadedForFirstTime = true;
    }

    auto *font = TTF_OpenFont(path, ptSize);

    if (font == nullptr)
    {
        return nullptr;
    }

    fontCache.insert_or_assign(path, font);

    return font;
}

/**
 * Load font from a read-only buffer.
 *
 * @param mem A pointer to a read-only buffer.
 * @param size The buffer size, in bytes.
 * @param ptSize The size of the font.
 */
inline auto LoadCachedFont(const void *mem, const int size,
                           const int ptSize = 24) -> TTF_Font *
{
    auto hash = MemHash(mem, size);

    auto match = fontCache.find(hash);

    if (match != fontCache.end())
    {
        return match->second;
    }

    if (!fontLoadedForFirstTime)
    {
        if (TTF_WasInit() == 0)
        {
            TTF_Init();
        }

        fontLoadedForFirstTime = true;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    if (rw == nullptr)
    {
        return nullptr;
    }

    auto *font = TTF_OpenFontRW(rw, 1, ptSize);

    if (font == nullptr)
    {
        SDL_RWclose(rw);

        return nullptr;
    }

    fontCache.insert_or_assign(hash, font);

    return font;
}

} // namespace HandcrankEngine
