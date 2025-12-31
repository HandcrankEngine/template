// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include "Utilities.hpp"

namespace HandcrankEngine
{

inline const int DEFAULT_FONT_SIZE = 24;

namespace
{
bool fontLoadedForFirstTime = false;

inline std::unordered_map<std::size_t, std::shared_ptr<TTF_Font>> fontCache =
    std::unordered_map<std::size_t, std::shared_ptr<TTF_Font>>();
} // namespace

struct FontDeleter
{
    void operator()(TTF_Font *font) const
    {
        if (font != nullptr)
        {
            TTF_CloseFont(font);
        }
    }
};

inline auto ClearFontCache() -> void { fontCache.clear(); }

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
inline auto LoadCachedFont(const char *path, int ptSize = DEFAULT_FONT_SIZE)
    -> TTF_Font *
{
    auto cacheKey = std::hash<std::string_view>{}(std::string_view(path)) ^
                    std::hash<int>{}(ptSize);

    auto match = fontCache.find(cacheKey);

    if (match != fontCache.end())
    {
        return match->second.get();
    }

    if (!fontLoadedForFirstTime)
    {
        if (TTF_WasInit() == 0)
        {
            TTF_Init();
        }

        fontLoadedForFirstTime = true;
    }

    auto font =
        std::shared_ptr<TTF_Font>(TTF_OpenFont(path, ptSize), FontDeleter{});

    if (font == nullptr)
    {
        return nullptr;
    }

    fontCache.insert_or_assign(cacheKey, font);

    return font.get();
}

/**
 * Load font from a read-only buffer.
 *
 * @param mem A pointer to a read-only buffer.
 * @param size The buffer size, in bytes.
 * @param ptSize The size of the font.
 */
inline auto LoadCachedFont(const void *mem, int size,
                           int ptSize = DEFAULT_FONT_SIZE) -> TTF_Font *
{
    auto cacheKey = MemHash(mem, size) ^ std::hash<int>{}(ptSize);

    auto match = fontCache.find(cacheKey);

    if (match != fontCache.end())
    {
        return match->second.get();
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

    auto font =
        std::shared_ptr<TTF_Font>(TTF_OpenFontRW(rw, 1, ptSize), FontDeleter{});

    if (font == nullptr)
    {
        SDL_RWclose(rw);

        return nullptr;
    }

    fontCache.insert_or_assign(cacheKey, font);

    return font.get();
}

} // namespace HandcrankEngine
