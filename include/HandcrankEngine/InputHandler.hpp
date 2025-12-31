// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <unordered_map>
#include <vector>

#include <SDL.h>

namespace HandcrankEngine
{

inline const int DEFAULT_KEY_MAP_SIZE = 250;
inline const int DEFAULT_MOUSE_BUTTON_MAP_SIZE = 48;
inline const int DEFAULT_CONNECTED_GAME_CONTROLLER_MAP_SIZE = 12;
inline const int DEFAULT_GAME_CONTROLLER_BUTTON_MAP_SIZE = 25;

class InputHandler
{
  protected:
    SDL_Event event;

    std::unordered_map<SDL_Keycode, bool> keyState;
    std::unordered_map<SDL_Keycode, bool> keyPressedState;
    std::unordered_map<SDL_Keycode, bool> keyReleasedState;

    SDL_FPoint mousePosition{};

    std::unordered_map<Uint8, bool> mouseState;
    std::unordered_map<Uint8, bool> mousePressedState;
    std::unordered_map<Uint8, bool> mouseReleasedState;

    std::unordered_map<Uint8, SDL_GameController *> connectedControllers;

    std::unordered_map<SDL_GameControllerButton, bool> controllerButtonState;
    std::unordered_map<SDL_GameControllerButton, bool>
        controllerButtonPressedState;
    std::unordered_map<SDL_GameControllerButton, bool>
        controllerButtonReleasedState;

  public:
    inline InputHandler();

    void HandleInputSetup();
    void HandleInputPollEvent(SDL_Event event);

    [[nodiscard]] inline auto IsKeyDown(SDL_Keycode keyCode) const -> bool;
    [[nodiscard]] inline auto
    IsKeyDown(const std::vector<SDL_Keycode> &keyCodes) const -> bool;
    [[nodiscard]] inline auto IsAnyKeyPressed() const -> bool;
    [[nodiscard]] inline auto IsKeyPressed(SDL_Keycode keyCode) const -> bool;
    [[nodiscard]] inline auto
    IsKeyPressed(const std::vector<SDL_Keycode> &keyCodes) const -> bool;
    [[nodiscard]] inline auto IsKeyReleased(SDL_Keycode keyCode) const -> bool;
    [[nodiscard]] inline auto
    IsKeyReleased(const std::vector<SDL_Keycode> &keyCodes) const -> bool;

    [[nodiscard]] inline auto GetMousePosition() const -> SDL_FPoint;

    [[nodiscard]] inline auto IsMouseButtonDown(Uint8 buttonIndex) const
        -> bool;
    [[nodiscard]] inline auto IsMouseButtonPressed(Uint8 buttonIndex) const
        -> bool;
    [[nodiscard]] inline auto IsMouseButtonReleased(Uint8 buttonIndex) const
        -> bool;

    [[nodiscard]] inline auto
    IsControllerButtonDown(SDL_GameControllerButton controllerButton) const
        -> bool;
    [[nodiscard]] inline auto IsControllerButtonDown(
        const std::vector<SDL_GameControllerButton> &controllerButtons) const
        -> bool;
    [[nodiscard]] inline auto IsAnyControllerButtonPressed() const -> bool;
    [[nodiscard]] inline auto
    IsControllerButtonPressed(SDL_GameControllerButton controllerButton) const
        -> bool;
    [[nodiscard]] inline auto IsControllerButtonPressed(
        const std::vector<SDL_GameControllerButton> &controllerButtons) const
        -> bool;
    [[nodiscard]] inline auto
    IsControllerButtonReleased(SDL_GameControllerButton controllerButton) const
        -> bool;
    [[nodiscard]] inline auto IsControllerButtonReleased(
        const std::vector<SDL_GameControllerButton> &controllerButtons) const
        -> bool;
};

InputHandler::InputHandler()
{
    keyState.reserve(DEFAULT_KEY_MAP_SIZE);
    keyPressedState.reserve(DEFAULT_KEY_MAP_SIZE);
    keyReleasedState.reserve(DEFAULT_KEY_MAP_SIZE);

    mouseState.reserve(DEFAULT_MOUSE_BUTTON_MAP_SIZE);
    mousePressedState.reserve(DEFAULT_MOUSE_BUTTON_MAP_SIZE);
    mouseReleasedState.reserve(DEFAULT_MOUSE_BUTTON_MAP_SIZE);

    connectedControllers.reserve(DEFAULT_CONNECTED_GAME_CONTROLLER_MAP_SIZE);

    controllerButtonState.reserve(DEFAULT_GAME_CONTROLLER_BUTTON_MAP_SIZE);
    controllerButtonPressedState.reserve(
        DEFAULT_GAME_CONTROLLER_BUTTON_MAP_SIZE);
    controllerButtonReleasedState.reserve(
        DEFAULT_GAME_CONTROLLER_BUTTON_MAP_SIZE);
}

void InputHandler::HandleInputSetup()
{
    keyPressedState.clear();
    keyReleasedState.clear();

    mousePressedState.clear();
    mouseReleasedState.clear();

    controllerButtonPressedState.clear();
    controllerButtonReleasedState.clear();
}

void InputHandler::HandleInputPollEvent(const SDL_Event event)
{
    auto keyCode = event.key.keysym.sym;

    auto mouseButtonIndex = event.button.button;
    auto controllerButton = (SDL_GameControllerButton)event.cbutton.button;

    switch (event.type)
    {
    case SDL_KEYDOWN:
        keyPressedState[keyCode] = !keyState[keyCode];
        keyState[keyCode] = true;
        break;

    case SDL_KEYUP:
        keyState[keyCode] = false;
        keyReleasedState[keyCode] = true;
        break;

    case SDL_MOUSEMOTION:
        mousePosition.x = event.motion.x;
        mousePosition.y = event.motion.y;
        break;

    case SDL_MOUSEBUTTONDOWN:
        mousePressedState[mouseButtonIndex] = !mouseState[mouseButtonIndex];
        mouseState[mouseButtonIndex] = true;
        break;

    case SDL_MOUSEBUTTONUP:
        mouseState[mouseButtonIndex] = false;
        mouseReleasedState[mouseButtonIndex] = true;
        break;

    case SDL_CONTROLLERDEVICEADDED:
        if (SDL_IsGameController(event.cdevice.which) == SDL_TRUE)
        {
            auto *controller = SDL_GameControllerOpen(event.cdevice.which);

            auto id = SDL_JoystickInstanceID(
                SDL_GameControllerGetJoystick(controller));

            connectedControllers[id] = controller;
        }

        break;

    case SDL_CONTROLLERDEVICEREMOVED:
        for (const auto &[id, controller] : connectedControllers)
        {
            if (controller != nullptr)
            {
                auto controllerInstanceId = SDL_JoystickInstanceID(
                    SDL_GameControllerGetJoystick(controller));

                if (controllerInstanceId == event.cdevice.which)
                {
                    SDL_GameControllerClose(controller);

                    connectedControllers.erase(id);

                    break;
                }
            }
        }

        break;

    case SDL_CONTROLLERBUTTONDOWN:
        controllerButtonPressedState[controllerButton] =
            !controllerButtonState[controllerButton];
        controllerButtonState[controllerButton] = true;
        break;
    case SDL_CONTROLLERBUTTONUP:
        controllerButtonState[controllerButton] = false;
        controllerButtonReleasedState[controllerButton] = true;
        break;

    default:
        break;
    }
}

auto InputHandler::IsKeyDown(const SDL_Keycode keyCode) const -> bool
{
    auto result = keyState.find(keyCode);

    return result != keyState.end() && result->second;
};

auto InputHandler::IsKeyDown(const std::vector<SDL_Keycode> &keyCodes) const
    -> bool
{
    return std::any_of(keyCodes.begin(), keyCodes.end(),
                       [this](SDL_Keycode val) { return IsKeyDown(val); });
};

auto InputHandler::IsAnyKeyPressed() const -> bool
{
    auto result = keyPressedState.size() > 0;

    return result;
};

auto InputHandler::IsKeyPressed(const SDL_Keycode keyCode) const -> bool
{
    auto result = keyPressedState.find(keyCode);

    return result != keyPressedState.end() && result->second;
};

auto InputHandler::IsKeyPressed(const std::vector<SDL_Keycode> &keyCodes) const
    -> bool
{
    return std::any_of(keyCodes.begin(), keyCodes.end(),
                       [this](SDL_Keycode val) { return IsKeyPressed(val); });
};

auto InputHandler::IsKeyReleased(const SDL_Keycode keyCode) const -> bool
{
    auto result = keyReleasedState.find(keyCode);

    return result != keyReleasedState.end() && result->second;
};

auto InputHandler::IsKeyReleased(const std::vector<SDL_Keycode> &keyCodes) const
    -> bool
{
    return std::any_of(keyCodes.begin(), keyCodes.end(),
                       [this](SDL_Keycode val) { return IsKeyReleased(val); });
};

auto InputHandler::GetMousePosition() const -> SDL_FPoint
{
    return mousePosition;
};

auto InputHandler::IsMouseButtonDown(const Uint8 buttonIndex) const -> bool
{
    auto result = mouseState.find(buttonIndex);

    return result != mouseState.end() && result->second;
};

auto InputHandler::IsMouseButtonPressed(const Uint8 buttonIndex) const -> bool
{
    auto result = mousePressedState.find(buttonIndex);

    return result != mousePressedState.end() && result->second;
};

auto InputHandler::IsMouseButtonReleased(const Uint8 buttonIndex) const -> bool
{
    auto result = mouseReleasedState.find(buttonIndex);

    return result != mouseReleasedState.end() && result->second;
};

auto InputHandler::IsControllerButtonDown(
    const SDL_GameControllerButton controllerButton) const -> bool
{
    auto result = controllerButtonState.find(controllerButton);

    return result != controllerButtonState.end() && result->second;
};

auto InputHandler::IsControllerButtonDown(
    const std::vector<SDL_GameControllerButton> &controllerButtons) const
    -> bool
{
    return std::any_of(controllerButtons.begin(), controllerButtons.end(),
                       [this](SDL_GameControllerButton val)
                       { return IsControllerButtonDown(val); });
};

auto InputHandler::IsAnyControllerButtonPressed() const -> bool
{
    auto result = controllerButtonPressedState.size() > 0;

    return result;
};

auto InputHandler::IsControllerButtonPressed(
    const SDL_GameControllerButton controllerButton) const -> bool
{
    auto result = controllerButtonPressedState.find(controllerButton);

    return result != controllerButtonPressedState.end() && result->second;
};

auto InputHandler::IsControllerButtonPressed(
    const std::vector<SDL_GameControllerButton> &controllerButtons) const
    -> bool
{
    return std::any_of(controllerButtons.begin(), controllerButtons.end(),
                       [this](SDL_GameControllerButton val)
                       { return IsControllerButtonPressed(val); });
};

auto InputHandler::IsControllerButtonReleased(
    const SDL_GameControllerButton controllerButton) const -> bool
{
    auto result = controllerButtonReleasedState.find(controllerButton);

    return result != controllerButtonReleasedState.end() && result->second;
};

auto InputHandler::IsControllerButtonReleased(
    const std::vector<SDL_GameControllerButton> &controllerButtons) const
    -> bool
{
    return std::any_of(controllerButtons.begin(), controllerButtons.end(),
                       [this](SDL_GameControllerButton val)
                       { return IsControllerButtonReleased(val); });
};

} // namespace HandcrankEngine
