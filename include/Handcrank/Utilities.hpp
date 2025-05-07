// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <algorithm>
#include <functional>
#include <regex>
#include <string>

namespace Handcrank
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

inline auto LeftPad(const std::string &content, const char pad, int length)
    -> std::string
{
    return std::string(length - content.size(), pad) + content;
}

inline auto RightPad(const std::string &content, const char pad, int length)
    -> std::string
{
    return content + std::string(length - content.size(), pad);
}

inline auto Lerp(float a, float b, float t) -> float
{
    return ((1 - t) * a) + (b * t);
}

inline auto InverseLerp(float a, float b, float v) -> float
{
    return std::clamp(((v - a) / (b - a)), 0.0F, 1.0F);
}

inline auto RandomNumberRange(int min, int max) -> int
{
    return (rand() % (max - min + 1)) + min;
}

inline auto RandomBoolean() -> bool { return rand() > (RAND_MAX / 2); }

template <typename T> auto GetClassNameSimple(const T &obj) -> std::string
{
    std::string rawName = typeid(obj).name();

    std::regex pattern("([0-9]+)$");

    std::vector<std::string> characters;

    for (int i = (int)rawName.length(); i > 0; i -= 1)
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
                    if (length == (int)rawName.length() - (i + 1))
                    {
                        return rawName.substr(i + 1);
                    }
                }
            }
        }
    }

    return rawName;
}

inline auto MemHash(const void *mem, const int size) -> std::string
{
    auto hash = std::hash<std::string_view>{}(
        std::string_view(static_cast<const char *>(mem), size));
    return std::to_string(hash);
}

} // namespace Handcrank
