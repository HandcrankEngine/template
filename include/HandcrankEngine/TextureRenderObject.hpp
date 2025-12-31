// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "HandcrankEngine.hpp"
#include "TextureCache.hpp"

namespace HandcrankEngine
{

class TextureRenderObject : public RenderObject
{
  protected:
    SDL_Texture *texture = nullptr;

    int textureWidth;
    int textureHeight;

  public:
    using RenderObject::RenderObject;

    /**
     * Set texture from an existing texture reference.
     *
     * @param texture A texture.
     */
    void SetTexture(SDL_Texture *texture)
    {
        this->texture = texture;

        UpdateRectSizeFromTexture();
    }

    /**
     * Load texture from a path.
     *
     * @param renderer A structure representing rendering state.
     * @param path File path to texture file.
     */
    void LoadTexture(SDL_Renderer *renderer, const char *path)
    {
        texture = LoadCachedTexture(renderer, path);

        UpdateRectSizeFromTexture();
    }

    /**
     * Load texture from a path.
     *
     * @param renderer A structure representing rendering state.
     * @param path File path to texture file.
     * @param color The color to use as the transparent color key.
     */
    void LoadTransparentTexture(SDL_Renderer *renderer, const char *path,
                                const SDL_Color colorKey)
    {
        texture = LoadCachedTransparentTexture(renderer, path, colorKey);

        UpdateRectSizeFromTexture();
    }

    /**
     * Load texture from a read-only buffer.
     *
     * @param renderer A structure representing rendering state.
     * @param mem A pointer to a read-only buffer.
     * @param size The buffer size, in bytes.
     */
    void LoadTexture(SDL_Renderer *renderer, const void *mem, int size)
    {
        texture = LoadCachedTexture(renderer, mem, size);

        UpdateRectSizeFromTexture();
    }

    /**
     * Load texture from a read-only buffer.
     *
     * @param renderer A structure representing rendering state.
     * @param mem A pointer to a read-only buffer.
     * @param size The buffer size, in bytes.
     * @param color The color to use as the transparent color key.
     */
    void LoadTransparentTexture(SDL_Renderer *renderer, const void *mem,
                                int size, const SDL_Color colorKey)
    {
        texture = LoadCachedTransparentTexture(renderer, mem, size, colorKey);

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
        texture = LoadCachedTexture(renderer, content.c_str(), content.size());

        UpdateRectSizeFromTexture();
    }

    void UpdateRectSizeFromTexture()
    {
        if (texture == nullptr)
        {
            return;
        }

        SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth,
                         &textureHeight);

        rect.w = textureWidth;
        rect.h = textureHeight;
    }
};

} // namespace HandcrankEngine
