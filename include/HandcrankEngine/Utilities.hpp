// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>

#include <algorithm>
#include <functional>
#include <regex>
#include <string>

namespace HandcrankEngine
{

inline auto TryParseInt(const std::string &value, int &result) -> bool
{
    try
    {
        auto number = std::stoi(value);

        result = number;

        return true;
    }
    catch ([[maybe_unused]] std::invalid_argument const &_)
    {
        // String failed to parse as an int.
    }

    return false;
}

inline auto TryParseInt(const std::string &value) -> bool
{
    try
    {
        std::stoi(value);

        return true;
    }
    catch ([[maybe_unused]] std::invalid_argument const &_)
    {
        // String failed to parse as an int.
    }

    return false;
}

inline auto LeftPad(const std::string &content, const char pad,
                    const size_t length) -> std::string
{
    if (content.size() >= length)
    {
        return content;
    }

    return std::string(length - content.size(), pad) + content;
}

inline auto RightPad(const std::string &content, const char pad,
                     const size_t length) -> std::string
{
    if (content.size() >= length)
    {
        return content;
    }

    return content + std::string(length - content.size(), pad);
}

inline auto Lerp(float a, float b, float t) -> float
{
    return ((1 - t) * a) + (b * t);
}

inline auto InverseLerp(float a, float b, float v) -> float
{
    if (a == b)
    {
        return 0.0F;
    }

    return std::clamp(((v - a) / (b - a)), 0.0F, 1.0F);
}

inline auto RandomNumberRange(int min, int max) -> int
{
    return (rand() % (max - min + 1)) + min;
}

inline auto RandomColorRange(const SDL_Color min, const SDL_Color max)
    -> SDL_Color
{
    return SDL_Color{(Uint8)RandomNumberRange(min.r, max.r),
                     (Uint8)RandomNumberRange(min.g, max.g),
                     (Uint8)RandomNumberRange(min.b, max.b),
                     (Uint8)RandomNumberRange(min.a, max.a)};
}

inline auto RandomBoolean() -> bool { return rand() > (RAND_MAX / 2); }

template <typename T> auto GetClassNameSimple(const T &obj) -> std::string
{
    std::string rawName = typeid(obj).name();

    std::regex pattern("([0-9]+)$");

    std::vector<std::string> characters;

    for (size_t i = rawName.length(); i > 0; i -= 1)
    {
        if (TryParseInt(&rawName[i]))
        {
            std::smatch match;

            auto part = rawName.substr(0, i + 1);

            if (std::regex_search(part, match, pattern))
            {
                int length;

                if (TryParseInt(match[0], length))
                {
                    if (length == static_cast<int>(rawName.length()) - (i + 1))
                    {
                        return rawName.substr(i + 1);
                    }
                }
            }
        }
    }

    return rawName;
}

inline auto MemHash(const void *mem, const size_t size) -> size_t
{
    return std::hash<std::string_view>{}(
        std::string_view(static_cast<const char *>(mem), size));
}

inline auto ToString(const SDL_Rect &rect) -> std::string
{
    return "SDL_Rect(" + std::to_string(rect.x) + ", " +
           std::to_string(rect.y) + ", " + std::to_string(rect.w) + ", " +
           std::to_string(rect.h) + ")";
}

inline auto ToString(const SDL_FRect &rect) -> std::string
{
    return "SDL_FRect(" + std::to_string(rect.x) + ", " +
           std::to_string(rect.y) + ", " + std::to_string(rect.w) + ", " +
           std::to_string(rect.h) + ")";
}

inline auto GenerateTextureQuad(std::vector<SDL_Vertex> &vertices,
                                std::vector<int> &indices,
                                const SDL_FRect &destRect,
                                const SDL_FRect &srcRect,
                                const SDL_Color &color, float textureWidth,
                                float textureHeight) -> void
{
    const size_t vertices_size = 4;
    const size_t indices_size = 6;

    auto index = vertices.size();

    vertices.reserve(vertices.size() + vertices_size);

    vertices.emplace_back(
        SDL_Vertex{{destRect.x, destRect.y},
                   color,
                   {srcRect.x / textureWidth, srcRect.y / textureHeight}});
    vertices.emplace_back(SDL_Vertex{
        {destRect.x + destRect.w, destRect.y},
        color,
        {(srcRect.x + srcRect.w) / textureWidth, srcRect.y / textureHeight}});
    vertices.emplace_back(
        SDL_Vertex{{destRect.x + destRect.w, destRect.y + destRect.h},
                   color,
                   {(srcRect.x + srcRect.w) / textureWidth,
                    (srcRect.y + srcRect.h) / textureHeight}});
    vertices.emplace_back(SDL_Vertex{
        {destRect.x, destRect.y + destRect.h},
        color,
        {srcRect.x / textureWidth, (srcRect.y + srcRect.h) / textureHeight}});

    indices.reserve(indices.size() + indices_size);

    indices.emplace_back(static_cast<int>(index));
    indices.emplace_back(static_cast<int>(index + 1));
    indices.emplace_back(static_cast<int>(index + 2));

    indices.emplace_back(static_cast<int>(index));
    indices.emplace_back(static_cast<int>(index + 2));
    indices.emplace_back(static_cast<int>(index + 3));
}

inline auto UpdateTextureQuad(SDL_Vertex *vertices_ptr,
                              const SDL_FRect &destRect) -> void
{
    vertices_ptr[0].position.x = destRect.x;
    vertices_ptr[0].position.y = destRect.y;

    vertices_ptr[1].position.x = destRect.x + destRect.w;
    vertices_ptr[1].position.y = destRect.y;

    vertices_ptr[2].position.x = destRect.x + destRect.w;
    vertices_ptr[2].position.y = destRect.y + destRect.h;

    vertices_ptr[3].position.x = destRect.x;
    vertices_ptr[3].position.y = destRect.y + destRect.h;
}

} // namespace HandcrankEngine
