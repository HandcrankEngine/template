// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>

#include "ImageRenderObject.hpp"
#include "Vector2.hpp"

namespace HandcrankEngine
{

const double DEFAULT_FRAME_SPEED = 0.1;

class SpriteRenderObject : public ImageRenderObject
{
  private:
    std::vector<SDL_Rect> spriteFrames;

    size_t frame = 0;

    double frameSpeed = DEFAULT_FRAME_SPEED;

    bool isPlaying = false;

    bool isLooping = true;

    double nextTick = 0;

  public:
    using ImageRenderObject::ImageRenderObject;

    void Play() { isPlaying = true; }
    void PlayOnce()
    {
        frame = 0;
        isPlaying = true;
        isLooping = false;
    }
    void Pause() { isPlaying = false; }
    void Resume() { isPlaying = true; }
    void Stop()
    {
        frame = 0;

        CalculateRect();

        isPlaying = false;
    }
    auto IsPlaying() const -> bool { return isPlaying; }

    auto GetFrame() const -> size_t { return frame; }

    void SetFrameSpeed(double frameSpeed) { this->frameSpeed = frameSpeed; }

    void SetFrames(const std::vector<SDL_Rect> &spriteFrames)
    {
        this->spriteFrames = spriteFrames;
    }

    void SetFrameIndex(int frameIndex)
    {
        auto spriteFramesSize = size(spriteFrames);

        if (frameIndex < spriteFramesSize)
        {
            frame = frameIndex;
        }
    }

    void CalculateFrames(float width, float height, int columns, int rows,
                         const Vector2 &padding, const Vector2 &offset)
    {
        spriteFrames.clear();
        spriteFrames.reserve(columns * rows);

        for (auto y = 0; y < rows; y += 1)
        {
            for (auto x = 0; x < columns; x += 1)
            {
                AddFrame(
                    {static_cast<int>(offset.x + (x * (width + padding.x))),
                     static_cast<int>(offset.y + (y * (height + padding.y))),
                     static_cast<int>(width), static_cast<int>(height)});
            }
        }

        CalculateRect();
    }

    void AddFrame(const SDL_Rect &rect) { spriteFrames.emplace_back(rect); }

    void ClearFrames() { spriteFrames.clear(); }

    void CalculateRect()
    {
        if (frame >= 0 && frame < spriteFrames.size())
        {
            SetSrcRect(spriteFrames.at(frame));
        }

        if (srcRectSet)
        {
            rect.w = srcRect.w;
            rect.h = srcRect.h;
        }
    }

    void InternalUpdate(double deltaTime) override
    {
        ImageRenderObject::InternalUpdate(deltaTime);

        if (!isPlaying)
        {
            return;
        }

        auto spriteFramesSize = size(spriteFrames);

        nextTick += deltaTime;

        if (nextTick < frameSpeed)
        {
            return;
        }

        frame += 1;

        if (frame == spriteFramesSize)
        {
            if (!isLooping)
            {
                isPlaying = false;
            }
            else
            {
                frame = 0;
            }
        }

        nextTick = 0;
    }

    void Render(SDL_Renderer *renderer) override
    {
        if (!CanRender())
        {
            return;
        }

        CalculateRect();

        ImageRenderObject::Render(renderer);
    }
};

} // namespace HandcrankEngine
