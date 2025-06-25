// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Handcrank.hpp"

namespace Handcrank
{

/**
 * Load font from a path.
 *
 * @param path File path to font file.
 * @param ptSize The size of the font.
 */
inline auto SDL_LoadFont(const char *path, const int ptSize = 24)
    -> std::shared_ptr<TTF_Font>
{
    if (TTF_WasInit() == 0)
    {
        TTF_Init();
    }

    auto *font = TTF_OpenFont(path, ptSize);

    if (font == nullptr)
    {
        return nullptr;
    }

    return {font, TTF_CloseFont};
}

/**
 * Load font from a read-only buffer.
 *
 * @param mem A pointer to a read-only buffer.
 * @param size The buffer size, in bytes.
 * @param ptSize The size of the font.
 */
inline auto SDL_LoadFont(const void *mem, const int size, const int ptSize = 24)
    -> std::shared_ptr<TTF_Font>
{
    if (TTF_WasInit() == 0)
    {
        TTF_Init();
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

    return {font, TTF_CloseFont};
}

class TextRenderObject : public RenderObject
{
  protected:
    std::shared_ptr<TTF_Font> font;

    SDL_Color color{MAX_R, MAX_G, MAX_B, MAX_ALPHA};

    std::string text;

    SDL_Surface *textSurface = nullptr;

    SDL_Texture *textTexture = nullptr;

  public:
    explicit TextRenderObject()
    {
        if (TTF_WasInit() == 0)
        {
            TTF_Init();
        }
    };

    explicit TextRenderObject(const float x, const float y) : RenderObject(x, y)
    {
        if (TTF_WasInit() == 0)
        {
            TTF_Init();
        }
    };
    explicit TextRenderObject(const float x, const float y, const float w,
                              const float h)
        : RenderObject(x, y, w, h)
    {
        if (TTF_WasInit() == 0)
        {
            TTF_Init();
        }
    };

    ~TextRenderObject()
    {
        if (textTexture != nullptr)
        {
            SDL_DestroyTexture(textTexture);
        }
        if (textSurface != nullptr)
        {
            SDL_FreeSurface(textSurface);
        }
    };

    /**
     * Set text font.
     *
     * @param font Font value to set.
     */
    void SetFont(const std::shared_ptr<TTF_Font> &font) { this->font = font; }

    /**
     * Load font from a path.
     *
     * @param path File path to font file.
     * @param ptSize The size of the font.
     *
     * @deprecated DEVELOPMENT USE ONLY! Use SDL_LoadFont to load
     * fonts in a release build.
     */
    void LoadFont(const char *path, const int ptSize = 24)
    {
        font = SDL_LoadFont(path, ptSize);
    }

    /**
     * Load font from a read-only buffer.
     *
     * @param mem A pointer to a read-only buffer.
     * @param size The buffer size, in bytes.
     * @param ptSize The size of the font.
     */
    void LoadFontRW(const void *mem, const int size, const int ptSize = 24)
    {
        font = SDL_LoadFont(mem, size, ptSize);
    }

    /**
     * Set text color.
     *
     * @param color Color value to set.
     */
    void SetColor(const SDL_Color color) { this->color = color; }

    /**
     * Set text content.
     *
     * @param text Text value to set.
     */
    void SetText(const std::string &text)
    {
        if (font == nullptr)
        {
            std::cerr << "ERROR! Missing font reference.\n";

            return;
        }

        this->text = text;

        if (textTexture != nullptr)
        {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }
        if (textSurface != nullptr)
        {
            SDL_FreeSurface(textSurface);
            textSurface = nullptr;
        }

        textSurface =
            TTF_RenderText_Blended(font.get(), this->text.c_str(), color);

        if (textSurface == nullptr)
        {
            std::cerr << "ERROR! Failed to generate text surface.\n";

            return;
        }

        rect.w = textSurface->w;
        rect.h = textSurface->h;

        textTexture = nullptr;
    }

    /**
     * Set text content.
     *
     * @param text Text value to set.
     */
    void SetWrappedText(const std::string &text)
    {
        if (font == nullptr)
        {
            std::cerr << "ERROR! Missing font reference.\n";

            return;
        }

        this->text = text;

        if (textTexture != nullptr)
        {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }
        if (textSurface != nullptr)
        {
            SDL_FreeSurface(textSurface);
            textSurface = nullptr;
        }

        textSurface = TTF_RenderText_Blended_Wrapped(
            font.get(), this->text.c_str(), color, rect.w);

        if (textSurface == nullptr)
        {
            std::cerr << "ERROR! Failed to generate text surface.\n";

            return;
        }

        rect.w = textSurface->w;
        rect.h = textSurface->h;

        textTexture = nullptr;
    }

    auto GetText() -> std::string { return text; }

    /**
     * Render text to the scene.
     *
     * @param renderer A structure representing rendering state.
     */
    void Render(SDL_Renderer *renderer) override
    {
        if (!CanRender())
        {
            return;
        }

        if (textTexture == nullptr && textSurface != nullptr)
        {
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        }

        auto transformedRect = GetTransformedRect();

        SDL_RenderCopyF(renderer, textTexture, nullptr, &transformedRect);

        RenderObject::Render(renderer);
    }
};

} // namespace Handcrank
