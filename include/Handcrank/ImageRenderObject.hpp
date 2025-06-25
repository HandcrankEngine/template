// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "Handcrank.hpp"
#include "Utilities.hpp"

namespace Handcrank
{

namespace
{
std::unordered_map<std::string, SDL_Texture *> textureCache =
    std::unordered_map<std::string, SDL_Texture *>();
}

inline auto ClearTextureCache() -> void
{
    for (const auto &texture : textureCache)
    {
        SDL_DestroyTexture(texture.second);
    }

    textureCache.clear();
}

/**
 * Load texture from a path.
 *
 * @param renderer A structure representing rendering state.
 * @param path File path to texture file.
 */

inline auto SDL_LoadTexture(SDL_Renderer *renderer, const char *path)
    -> SDL_Texture *
{
    if (textureCache.find(path) != textureCache.end())
    {
        return textureCache.find(path)->second;
    }

    auto *surface = IMG_Load(path);

    if (surface == nullptr)
    {
        return nullptr;
    }

    auto *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    if (texture == nullptr)
    {
        return nullptr;
    }

    textureCache.insert_or_assign(path, texture);

    return texture;
}

/**
 * Load texture from a read-only buffer.
 *
 * @param renderer A structure representing rendering state.
 * @param mem A pointer to a read-only buffer.
 * @param size The buffer size, in bytes.
 */
inline auto SDL_LoadTexture(SDL_Renderer *renderer, const void *mem,
                            const int size) -> SDL_Texture *
{
    auto hash = MemHash(mem, size);

    if (textureCache.find(hash) != textureCache.end())
    {
        return textureCache.find(hash)->second;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    auto *surface =
        IMG_isSVG(rw) == SDL_TRUE ? IMG_LoadSVG_RW(rw) : IMG_Load_RW(rw, 1);

    if (surface == nullptr)
    {
        return nullptr;
    }

    auto *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    if (texture == nullptr)
    {
        return nullptr;
    }

    textureCache.insert_or_assign(hash, texture);

    return texture;
}

class ImageRenderObject : public RenderObject
{
  protected:
    SDL_Texture *texture;

    SDL_Rect srcRect = SDL_Rect();

    bool srcRectSet = false;

    SDL_FPoint centerPoint = SDL_FPoint();

    SDL_Color tintColor = SDL_Color{MAX_R, MAX_G, MAX_B, MAX_ALPHA};

    int alpha = MAX_ALPHA;

    SDL_RendererFlip flip = SDL_FLIP_NONE;

  public:
    explicit ImageRenderObject() = default;
    explicit ImageRenderObject(const float x, const float y)
        : RenderObject(x, y) {};
    explicit ImageRenderObject(const float x, const float y, const float w,
                               const float h)
        : RenderObject(x, y, w, h) {};

    ~ImageRenderObject() = default;

    /**
     * Set texture from an existing texture reference.
     *
     * @param texture A texture.
     */
    void SetTexture(SDL_Texture *texture)
    {
        if (this->texture != nullptr)
        {
            SDL_DestroyTexture(this->texture);
            this->texture = nullptr;
        }

        this->texture = texture;

        UpdateRectSizeFromTexture();
    }

    /**
     * Load texture from a path.
     *
     * @param renderer A structure representing rendering state.
     * @param path File path to texture file.
     *
     * @deprecated DEVELOPMENT USE ONLY! Use LoadTexture to load textures in a
     * release build.
     */
    void LoadTexture(SDL_Renderer *renderer, const char *path)
    {
        if (texture != nullptr)
        {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }

        texture = SDL_LoadTexture(renderer, path);

        UpdateRectSizeFromTexture();
    }

    /**
     * Load texture from a read-only buffer.
     *
     * @param renderer A structure representing rendering state.
     * @param mem A pointer to a read-only buffer.
     * @param size The buffer size, in bytes.
     */
    void LoadTexture(SDL_Renderer *renderer, const void *mem, const int size)
    {
        if (texture != nullptr)
        {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }

        texture = SDL_LoadTexture(renderer, mem, size);

        UpdateRectSizeFromTexture();
    }

    /**
     * Load SVG texture from a string.
     *
     * @param renderer A structure representing rendering state.
     * @param content Full SVG string including <svg></svg> tags.
     */
    void LoadSVGString(SDL_Renderer *renderer, const std::string &content)
    {
        if (texture != nullptr)
        {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }

        texture = SDL_LoadTexture(renderer, content.c_str(), content.size());

        UpdateRectSizeFromTexture();
    }

    void UpdateRectSizeFromTexture()
    {
        int textureWidth;
        int textureHeight;

        SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth,
                         &textureHeight);

        rect.w = textureWidth;
        rect.h = textureHeight;
    }

    void SetSrcRect(const SDL_Rect srcRect)
    {
        this->srcRect.x = srcRect.x;
        this->srcRect.y = srcRect.y;
        this->srcRect.w = srcRect.w;
        this->srcRect.h = srcRect.h;

        srcRectSet = true;
    }

    void SetSrcRect(const int x, const int y, const int w, const int h)
    {
        this->srcRect.x = x;
        this->srcRect.y = y;
        this->srcRect.w = w;
        this->srcRect.h = h;

        srcRectSet = true;
    }

    void SetFlip(const SDL_RendererFlip flip) { this->flip = flip; }

    void SetTintColor(const SDL_Color tintColor)
    {
        this->tintColor.r = tintColor.r;
        this->tintColor.g = tintColor.g;
        this->tintColor.b = tintColor.b;
    }

    void SetTintColor(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        this->tintColor.r = r;
        this->tintColor.g = g;
        this->tintColor.b = b;
    }

    [[nodiscard]] auto GetTintColor() const -> const SDL_Color &
    {
        return tintColor;
    }

    void SetAlpha(const int alpha) { this->alpha = alpha; }

    [[nodiscard]] auto GetAlpha() const -> int { return alpha; }

    /**
     * Render image to the scene.
     *
     * @param renderer A structure representing rendering state.
     */
    void Render(SDL_Renderer *renderer) override
    {
        if (!CanRender())
        {
            return;
        }

        auto transformedRect = GetTransformedRect();

        SDL_SetTextureColorMod(texture, tintColor.r, tintColor.g, tintColor.b);

        SDL_SetTextureAlphaMod(texture, alpha);

        SDL_RenderCopyExF(renderer, texture, srcRectSet ? &srcRect : nullptr,
                          &transformedRect, 0, &centerPoint, flip);

        RenderObject::Render(renderer);
    }
};

} // namespace Handcrank
