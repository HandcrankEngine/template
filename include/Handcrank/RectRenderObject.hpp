// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>

#include "Handcrank.hpp"

namespace Handcrank
{

class RectRenderObject : public RenderObject
{
  protected:
    SDL_Color borderColor = SDL_Color();

    bool borderColorSet = false;

    SDL_Color fillColor = SDL_Color();

    bool fillColorSet = false;

    SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;

  public:
    explicit RectRenderObject() = default;
    explicit RectRenderObject(const float x, const float y)
        : RenderObject(x, y) {};
    explicit RectRenderObject(const float x, const float y, const float w,
                              const float h)
        : RenderObject(x, y, w, h) {};

    ~RectRenderObject() = default;

    /**
     * Set rect border color.
     *
     * @param borderColor Color value to set.
     */
    void SetBorderColor(const SDL_Color borderColor)
    {
        this->borderColor.r = borderColor.r;
        this->borderColor.g = borderColor.g;
        this->borderColor.b = borderColor.b;
        this->borderColor.a = borderColor.a;

        borderColorSet = true;
    }

    void SetBorderColor(const Uint8 r, const Uint8 g, const Uint8 b,
                        const Uint8 a)
    {
        borderColor.r = r;
        borderColor.g = g;
        borderColor.b = b;
        borderColor.a = a;

        borderColorSet = true;
    }

    [[nodiscard]] auto GetBorderColor() const -> const SDL_Color &
    {
        return borderColor;
    }

    /**
     * Set rect fill color.
     *
     * @param fillColor Color value to set.
     */
    void SetFillColor(const SDL_Color fillColor)
    {
        this->fillColor.r = fillColor.r;
        this->fillColor.g = fillColor.g;
        this->fillColor.b = fillColor.b;
        this->fillColor.a = fillColor.a;

        fillColorSet = true;
    }

    void SetFillColor(const Uint8 r, const Uint8 g, const Uint8 b,
                      const Uint8 a)
    {
        fillColor.r = r;
        fillColor.g = g;
        fillColor.b = b;
        fillColor.a = a;

        fillColorSet = true;
    }

    [[nodiscard]] auto GetFillColor() const -> const SDL_Color &
    {
        return fillColor;
    }

    /**
     * Render rect to the scene.
     *
     * @param renderer A structure representing rendering state.
     */
    void Render(SDL_Renderer *renderer) override
    {
        if (!CanRender())
        {
            return;
        }

        SDL_SetRenderDrawBlendMode(renderer, blendMode);

        auto transformedRect = GetTransformedRect();

        if (fillColorSet)
        {
            SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g,
                                   fillColor.b, fillColor.a);

            SDL_RenderFillRectF(renderer, &transformedRect);
        }

        if (borderColorSet)
        {
            SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g,
                                   borderColor.b, borderColor.a);

            SDL_RenderDrawRectF(renderer, &transformedRect);
        }

        RenderObject::Render(renderer);
    }
};

} // namespace Handcrank
