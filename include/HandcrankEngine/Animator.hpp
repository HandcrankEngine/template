// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include "Animation.hpp"
#include "HandcrankEngine.hpp"

namespace HandcrankEngine
{

inline const int DEFAULT_ANIMATION_VECTOR_SIZE = 10;

class Animator : public RenderObject
{
  public:
    enum class State : uint8_t
    {
        IDLE,
        RUNNING,
        PAUSED,
        COMPLETE
    };

    enum class Mode : uint8_t
    {
        PARALLEL,
        SEQUENCE
    };

  private:
    State currentState = State::IDLE;

    Mode mode = Mode::SEQUENCE;

    bool looping = false;

    std::vector<std::shared_ptr<Animation>> animations;

    size_t currentAnimationIndex = 0;

  public:
    using RenderObject::RenderObject;

    explicit Animator(const Mode mode)
    {
        this->mode = mode;
        this->animations.reserve(DEFAULT_ANIMATION_VECTOR_SIZE);
    }
    explicit Animator(const Mode mode, bool looping)
    {
        this->mode = mode;
        this->looping = looping;
        this->animations.reserve(DEFAULT_ANIMATION_VECTOR_SIZE);
    }

    void Start() override
    {
        if (animations.empty())
        {
            return;
        }

        if (mode == Mode::PARALLEL)
        {
            StartParallel();
        }
        else if (mode == Mode::SEQUENCE)
        {
            StartSequence();
        }
    }

    void Pause()
    {
        if (currentState == State::RUNNING)
        {
            for (const auto &animation : animations)
            {
                animation->Pause();
            }
        }

        currentState = State::PAUSED;
    }

    void Resume()
    {
        if (currentState == State::PAUSED)
        {
            for (const auto &animation : animations)
            {
                animation->Resume();
            }
        }

        currentState = State::RUNNING;
    }

    [[nodiscard]] auto GetState() const -> const State &
    {
        return currentState;
    }

    void SetState(State state) { currentState = state; }

    [[nodiscard]] auto GetMode() const -> const Mode & { return mode; }

    void SetMode(Mode mode) { this->mode = mode; }

    [[nodiscard]] auto IsLooping() const -> bool { return looping; }

    void SetIsLooping(bool looping) { this->looping = looping; }

    void AddAnimation(const std::shared_ptr<Animation> &animation)
    {
        animations.emplace_back(animation);
    }

    void AddDelay(float delay)
    {
        AddAnimation(std::make_shared<Animation>(
            [delay](double deltaTime, double elapsedTime)
            { return elapsedTime > delay ? 0 : 1; }));
    }

    void AddStep(const std::function<void()> &stepFunction)
    {
        AddAnimation(std::make_shared<Animation>(
            [stepFunction](double deltaTime, double elapsedTime)
            {
                stepFunction();

                return 0;
            }));
    }

    void Update(double deltaTime) override
    {
        if (!game->HasFocus())
        {
            return;
        }

        if (animations.empty())
        {
            return;
        }

        if (mode == Mode::PARALLEL)
        {
            UpdateParallel(deltaTime);
        }
        else if (mode == Mode::SEQUENCE)
        {
            UpdateSequence(deltaTime);
        }
    }

  private:
    void StartParallel()
    {
        for (const auto &animation : animations)
        {
            animation->Start();
        }

        currentState = State::RUNNING;
    }

    void StartSequence()
    {
        const auto &animation = animations.front();

        if (animation != nullptr)
        {
            animation->Start();
        }

        currentState = State::RUNNING;
    }

    void UpdateParallel(double deltaTime)
    {
        if (currentState != State::RUNNING)
        {
            return;
        }

        auto stillRunning = false;

        for (const auto &animation : animations)
        {
            if (animation->GetState() == Animation::State::RUNNING)
            {
                if (animation->Tick(deltaTime) != 0)
                {
                    stillRunning = true;
                }
            }
        }

        if (!stillRunning)
        {
            if (looping)
            {
                StartParallel();
            }
            else
            {
                currentState = State::COMPLETE;
            }
        }
    }

    void UpdateSequence(double deltaTime)
    {
        if (currentState != State::RUNNING)
        {
            return;
        }

        auto currentAnimation = animations[currentAnimationIndex];

        if (currentAnimation->GetState() == Animation::State::RUNNING)
        {
            if (currentAnimation->Tick(deltaTime) == 0)
            {
                currentAnimationIndex++;

                if (currentAnimationIndex >= animations.size())
                {
                    if (looping)
                    {
                        currentAnimationIndex = 0;
                    }
                    else
                    {
                        currentState = State::COMPLETE;

                        return;
                    }
                }

                currentAnimation = animations[currentAnimationIndex];
                currentAnimation->Start();
            }
        }
    }
};

} // namespace HandcrankEngine
