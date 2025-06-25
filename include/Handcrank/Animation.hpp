// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <functional>

namespace Handcrank
{

class Animation
{
  public:
    enum class State : uint8_t
    {
        IDLE,
        RUNNING,
        PAUSED,
        COMPLETE
    };

  private:
    State currentState = State::IDLE;

    double deframeStep;

    double elapsedTime;

    double elapsedDeframeTime;

    std::function<int(const double, const double)> tickFunction;

  public:
    Animation() = default;
    Animation(
        const std::function<int(const double, const double)> &tickFunction)
    {
        this->tickFunction = tickFunction;
    }
    Animation(
        const std::function<int(const double, const double)> &tickFunction,
        const double &deframeStep)
    {
        this->tickFunction = tickFunction;
        this->deframeStep = deframeStep;
    }

    ~Animation() = default;

    [[nodiscard]] auto GetState() const -> State { return currentState; }

    void SetState(State state) { currentState = state; }

    [[nodiscard]] auto GetElapsedTime() const -> double { return elapsedTime; }

    virtual void Start()
    {
        if (currentState == State::IDLE || currentState == State::COMPLETE)
        {
            elapsedTime = 0;
        }

        currentState = State::RUNNING;
    }

    void Pause()
    {
        if (currentState == State::RUNNING)
        {
            currentState = State::PAUSED;
        }
    }

    void Resume()
    {
        if (currentState == State::PAUSED)
        {
            currentState = State::RUNNING;
        }
    }

    virtual auto Tick(const double deltaTime) -> int
    {
        elapsedTime += deltaTime;

        if (elapsedDeframeTime > deframeStep)
        {
            auto result = tickFunction(deltaTime, elapsedTime);

            if (result == 0)
            {
                currentState = State::COMPLETE;
            }

            elapsedDeframeTime = 0;

            return result;
        }

        elapsedDeframeTime += deltaTime;

        return 1;
    }

    void
    SetTick(const std::function<int(const double, const double)> &tickFunction)
    {
        this->tickFunction = tickFunction;
    }

    void SetDeframeStep(const double &deframeStep)
    {
        this->deframeStep = deframeStep;
    }
};

} // namespace Handcrank
