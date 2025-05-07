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
std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textureCache =
    std::unordered_map<std::string, std::shared_ptr<SDL_Texture>>();
}

/**
 * Load texture from a path.
 *
 * @param renderer A structure representing rendering state.
 * @param path File path to texture file.
 *
 * @deprecated DEVELOPMENT USE ONLY! Use SDL_LoadTextureRW to load textures in a
 * release build.
 */

[[deprecated("DEVELOPMENT USE ONLY! Use SDL_LoadTextureRW to load textures in "
             "a release build.")]]
inline auto SDL_LoadTexture(const std::shared_ptr<SDL_Renderer> &renderer,
                            const char *path) -> std::shared_ptr<SDL_Texture>
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

    auto texture = std::shared_ptr<SDL_Texture>(
        SDL_CreateTextureFromSurface(renderer.get(), surface),
        SDL_DestroyTexture);

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
inline auto SDL_LoadTextureRW(const std::shared_ptr<SDL_Renderer> &renderer,
                              const void *mem, const int size)
    -> std::shared_ptr<SDL_Texture>
{
    auto hash = MemHash(mem, size);

    if (textureCache.find(hash) != textureCache.end())
    {
        return textureCache.find(hash)->second;
    }

    auto *rw = SDL_RWFromConstMem(mem, size);

    auto *surface = IMG_Load_RW(rw, 1);

    if (surface == nullptr)
    {
        return nullptr;
    }

    auto texture = std::shared_ptr<SDL_Texture>(
        SDL_CreateTextureFromSurface(renderer.get(), surface),
        SDL_DestroyTexture);

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
    std::shared_ptr<SDL_Texture> texture;

    std::shared_ptr<SDL_Rect> srcRect = std::make_shared<SDL_Rect>();

    bool srcRectSet = false;

    std::shared_ptr<SDL_FPoint> centerPoint = std::make_shared<SDL_FPoint>();

    std::shared_ptr<SDL_Color> tintColor =
        std::make_shared<SDL_Color>(SDL_Color{MAX_R, MAX_G, MAX_B, MAX_ALPHA});

    int alpha = MAX_ALPHA;

    SDL_RendererFlip flip = SDL_FLIP_NONE;

  public:
    explicit ImageRenderObject() = default;
    explicit ImageRenderObject(float x, float y) : RenderObject(x, y) {};
    explicit ImageRenderObject(float x, float y, float w, float h)
        : RenderObject(x, y, w, h) {};

    ~ImageRenderObject() = default;

    /**
     * Set texture from an existing texture reference.
     *
     * @param texture A texture.
     */
    void SetTexture(const std::shared_ptr<SDL_Texture> &texture)
    {
        this->texture.reset();
        this->texture = texture;

        UpdateRectSizeFromTexture();
    }

    /**
     * Load texture from a path.
     *
     * @param renderer A structure representing rendering state.
     * @param path File path to texture file.
     *
     * @deprecated DEVELOPMENT USE ONLY! Use LoadTextureRW to load textures in a
     * release build.
     */
    void LoadTexture(const std::shared_ptr<SDL_Renderer> &renderer,
                     const char *path)
    {
        texture.reset();
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
    void LoadTextureRW(const std::shared_ptr<SDL_Renderer> &renderer,
                       const void *mem, const int size)
    {
        texture.reset();
        texture = SDL_LoadTextureRW(renderer, mem, size);

        UpdateRectSizeFromTexture();
    }

    void UpdateRectSizeFromTexture() const
    {
        int textureWidth;
        int textureHeight;

        SDL_QueryTexture(texture.get(), nullptr, nullptr, &textureWidth,
                         &textureHeight);

        rect->w = textureWidth;
        rect->h = textureHeight;
    }

    void SetSrcRect(const SDL_Rect srcRect)
    {
        this->srcRect->x = srcRect.x;
        this->srcRect->y = srcRect.y;
        this->srcRect->w = srcRect.w;
        this->srcRect->h = srcRect.h;

        srcRectSet = true;
    }

    void SetSrcRect(const int x, const int y, const int w, const int h)
    {
        this->srcRect->x = x;
        this->srcRect->y = y;
        this->srcRect->w = w;
        this->srcRect->h = h;

        srcRectSet = true;
    }

    void SetFlip(const SDL_RendererFlip flip) { this->flip = flip; }

    void SetTintColor(const SDL_Color tintColor)
    {
        this->tintColor->r = tintColor.r;
        this->tintColor->g = tintColor.g;
        this->tintColor->b = tintColor.b;
    }

    void SetTintColor(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        this->tintColor->r = r;
        this->tintColor->g = g;
        this->tintColor->b = b;
    }

    [[nodiscard]] auto GetTintColor() const -> std::shared_ptr<SDL_Color>
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
    void Render(const std::shared_ptr<SDL_Renderer> &renderer) override
    {
        if (!CanRender())
        {
            return;
        }

        auto transformedRect = GetTransformedRect();

        SDL_SetTextureColorMod(texture.get(), tintColor->r, tintColor->g,
                               tintColor->b);

        SDL_SetTextureAlphaMod(texture.get(), alpha);

        SDL_RenderCopyExF(renderer.get(), texture.get(),
                          srcRectSet ? srcRect.get() : nullptr,
                          &transformedRect, 0, centerPoint.get(), flip);

        RenderObject::Render(renderer);
    }
};

} // namespace Handcrank
