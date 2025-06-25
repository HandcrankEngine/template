// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>

#include "ImageRenderObject.hpp"
#include "Vector2.hpp"

namespace Handcrank
{

const double DEFAULT_FRAME_SPEED = 0.1;

class SpriteRenderObject : public ImageRenderObject
{
  private:
    std::vector<SDL_Rect> spriteFrames = std::vector<SDL_Rect>();

    int frame = 0;

    double frameSpeed = DEFAULT_FRAME_SPEED;

    bool isPlaying = false;

    bool isLooping = true;

    double nextTick;

  public:
    explicit SpriteRenderObject() = default;
    explicit SpriteRenderObject(const float x, const float y)
        : ImageRenderObject(x, y) {};
    explicit SpriteRenderObject(const float x, const float y, const float w,
                                const float h)
        : ImageRenderObject(x, y, w, h) {};

    ~SpriteRenderObject() = default;

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
        isPlaying = false;
    }
    auto IsPlaying() const -> bool { return isPlaying; }

    auto GetFrame() const -> int { return frame; }

    void SetFrameSpeed(const double frameSpeed)
    {
        this->frameSpeed = frameSpeed;
    }

    void SetFrames(const std::vector<SDL_Rect> &spriteFrames)
    {
        this->spriteFrames = spriteFrames;
    }

    void SetFrameIndex(const int frameIndex)
    {
        auto spriteFramesSize = size(spriteFrames);

        if (frameIndex < spriteFramesSize)
        {
            frame = frameIndex;
        }
    }

    void CalculateFrames(const float width, const float height,
                         const int columns, const int rows,
                         const Vector2 &padding, const Vector2 &offset)
    {
        spriteFrames.clear();

        auto availableWidth = width - (padding.x * (columns - 1));
        auto availableHeight = height - (padding.y * (rows - 1));

        auto cellWidth = availableWidth / columns;
        auto cellHeight = availableHeight / rows;

        for (auto x = 0; x < columns; x++)
        {
            for (auto y = 0; y < rows; y++)
            {
                SDL_Rect rect;

                rect.x =
                    static_cast<int>((offset.x + x) * (cellWidth + padding.x));
                rect.y =
                    static_cast<int>((offset.y + y) * (cellHeight + padding.y));
                rect.w = static_cast<int>(cellWidth);
                rect.h = static_cast<int>(cellHeight);

                spriteFrames.push_back(rect);
            }
        }

        CalculateRect();
    }

    void CalculateRect()
    {
        auto srcRect = spriteFrames.at(frame);

        SetSrcRect(srcRect);

        rect.w = srcRect.w;
        rect.h = srcRect.h;
    }

    void InternalUpdate(const double deltaTime) override
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

        auto spriteFramesSize = size(spriteFrames);

        if (spriteFramesSize <= frame)
        {
            return;
        }

        CalculateRect();

        ImageRenderObject::Render(renderer);
    }
};

} // namespace Handcrank
