// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

// #define HANDCRANK_ENGINE_DEBUG 1

#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define HANDCRANK_ENGINE_VERSION_MAJOR 0
#define HANDCRANK_ENGINE_VERSION_MINOR 0
#define HANDCRANK_ENGINE_VERSION_PATCH 0

#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>

#include "AudioCache.hpp"
#include "FontCache.hpp"
#include "TextureCache.hpp"
#include "Utilities.hpp"

namespace HandcrankEngine
{

inline const int MAX_R = 255;
inline const int MAX_G = 255;
inline const int MAX_B = 255;
inline const int MAX_ALPHA = 255;

const double MILLISECONDS = 1000.0;

const double DEFAULT_FRAME_RATE = 60;
const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;
const float DEFAULT_RECT_WIDTH = 100;
const float DEFAULT_RECT_HEIGHT = 100;

class Game;
class RenderObject;

enum class RectAnchor : uint8_t
{
    TOP = 0x01,
    LEFT = 0x02,
    BOTTOM = 0x04,
    RIGHT = 0x08,
    VCENTER = 0x16,
    HCENTER = 0x32,
};

inline auto operator|(RectAnchor a, RectAnchor b) -> RectAnchor
{
    return static_cast<RectAnchor>(static_cast<uint8_t>(a) |
                                   static_cast<uint8_t>(b));
}

inline auto operator&(RectAnchor a, RectAnchor b) -> RectAnchor
{
    return static_cast<RectAnchor>(static_cast<uint8_t>(a) &
                                   static_cast<uint8_t>(b));
}

inline std::shared_ptr<SDL_Texture> debugRectTexture;

class Game
{
  private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    SDL_Rect viewport{};
    SDL_FRect viewportf{};

    SDL_Color clearColor{0, 0, 0, MAX_ALPHA};

    bool quit = false;

    SDL_Event event;

    std::unordered_map<SDL_Keycode, bool> keyState;
    std::unordered_map<SDL_Keycode, bool> keyPressedState;
    std::unordered_map<SDL_Keycode, bool> keyReleasedState;

    SDL_FPoint mousePosition{};

    std::unordered_map<Uint8, bool> mouseState;
    std::unordered_map<Uint8, bool> mousePressedState;
    std::unordered_map<Uint8, bool> mouseReleasedState;

    std::vector<std::shared_ptr<RenderObject>> children;
    std::vector<std::shared_ptr<RenderObject>> childrenBuffer;

    double elapsedTime = 0;
    double deltaTime = 0;
    double fixedUpdateDeltaTime = 0;

    double frameRate = DEFAULT_FRAME_RATE;

    Uint64 previousFrameStart = 0;
    double fps = 0;
    int framesThisSecond = 0;

    const double fixedFrameTime = 0.02;

    int width = DEFAULT_WINDOW_WIDTH;
    int height = DEFAULT_WINDOW_HEIGHT;

    bool focused = false;

#ifdef HANDCRANK_ENGINE_DEBUG
    bool debug = false;
#endif

  public:
    inline Game();
    inline ~Game();

    inline void AddChildObject(const std::shared_ptr<RenderObject> &child);

    template <typename T>
    inline auto GetChildrenByType(bool nested = false)
        -> std::vector<std::shared_ptr<T>>;
    template <typename T>
    inline auto GetChildByType(bool nested = false) -> std::shared_ptr<T>;
    inline auto GetChildCount() -> int;

    [[nodiscard]] inline auto GetWindow() -> SDL_Window *;
    [[nodiscard]] inline auto GetRenderer() -> SDL_Renderer *;
    [[nodiscard]] inline auto GetViewport() const -> const SDL_FRect &;

    inline auto SwitchToFullscreen() -> bool;
    inline auto SwitchToWindowedMode() -> bool;

    inline auto Setup() -> bool;

    inline void SetScreenSize(int _width, int _height);

    inline void SetTitle(const char *name);

    inline void SetClearColor(SDL_Color color);

    [[nodiscard]] inline auto GetWidth() const -> int;
    [[nodiscard]] inline auto GetHeight() const -> int;

    [[nodiscard]] inline auto HasFocus() const -> bool;

    [[nodiscard]] inline auto IsKeyDown(SDL_Keycode keyCode) const -> bool;
    [[nodiscard]] inline auto IsAnyKeyPressed() const -> bool;
    [[nodiscard]] inline auto IsKeyPressed(SDL_Keycode keyCode) const -> bool;
    [[nodiscard]] inline auto IsKeyReleased(SDL_Keycode keyCode) const -> bool;

    [[nodiscard]] inline auto GetMousePosition() const -> SDL_FPoint;

    [[nodiscard]] inline auto IsMouseButtonDown(Uint8 buttonIndex) const
        -> bool;
    [[nodiscard]] inline auto IsMouseButtonPressed(Uint8 buttonIndex) const
        -> bool;
    [[nodiscard]] inline auto IsMouseButtonReleased(Uint8 buttonIndex) const
        -> bool;

    [[nodiscard]] inline auto GetElapsedTime() const -> double;

    [[nodiscard]] inline auto GetFrameRate() const -> double;

    [[nodiscard]] inline auto GetFPS() const -> double;

    inline void SetFrameRate(double frameRate);

    [[nodiscard]] inline auto GetQuit() const -> bool;

    inline auto Run() -> int;

    inline void Loop();

#ifdef __EMSCRIPTEN__
    static inline void StaticLoop(void *userData)
    {
        auto *gameInstance = static_cast<Game *>(userData);

        if (gameInstance != nullptr)
        {
            gameInstance->Loop();
        }
    }
#endif

    inline void HandleInput();

    inline void CalculateDeltaTime();

    inline void FillChildrenBuffer();

    inline void Update();
    inline void FixedUpdate();

    inline void Render();

    inline void DestroyChildObjects();

    inline void Quit();

#ifdef HANDCRANK_ENGINE_DEBUG
    inline void ToggleDebug(bool state);
    inline void ToggleDebug();
    [[nodiscard]] inline auto IsDebug() const -> bool;
#endif
};

class RenderObject : public std::enable_shared_from_this<RenderObject>
{
  protected:
    inline static unsigned int count = 0;

    int index;

    std::string name;

    SDL_FRect rect = SDL_FRect();

    RectAnchor anchor = RectAnchor::TOP | RectAnchor::LEFT;

    float scale = 1;

    bool hasStarted = false;

    bool isEnabled = true;

    bool isMarkedForDestroy = false;

    bool isInputHovered = false;
    bool isInputActive = false;

    std::vector<std::shared_ptr<RenderObject>> children;
    std::vector<std::shared_ptr<RenderObject>> childrenBuffer;

  public:
    Game *game = nullptr;

    RenderObject *parent = nullptr;

    int z = 0;

    inline RenderObject();
    explicit RenderObject(const float x, const float y) { SetPosition(x, y); }
    explicit RenderObject(const float x, const float y, const float w,
                          const float h)
    {
        SetRect(x, y, w, h);
    }

    virtual inline ~RenderObject();

    inline void Enable();
    inline void Disable();
    [[nodiscard]] inline auto IsEnabled() const -> bool;

    [[nodiscard]] inline auto GetIndex() const -> int;

    [[nodiscard]] inline auto GetName() const -> std::string;

    [[nodiscard]] inline auto GetClassName() const -> std::string;

    inline void AddChildObject(const std::shared_ptr<RenderObject> &child);

    template <typename T>
    inline auto GetChildrenByType(bool nested = false)
        -> std::vector<std::shared_ptr<T>>;
    template <typename T>
    inline auto GetChildByType(bool nested = false) -> std::shared_ptr<T>;
    inline auto GetChildCount() -> int;

    inline void FillChildrenBuffer();

    virtual inline void Start();
    virtual inline void Update(double deltaTime);
    virtual inline void FixedUpdate(double deltaTime);

    virtual inline void OnMouseOver();
    virtual inline void OnMouseOut();
    virtual inline void OnMouseDown();
    virtual inline void OnMouseUp();

    virtual inline void InternalUpdate(double deltaTime);
    virtual inline void InternalFixedUpdate(double fixedDeltaTime);

    virtual inline void OnDestroy();

    [[nodiscard]] inline auto GetRect() const -> const SDL_FRect &;
    inline void SetRect(const SDL_FRect &rect);
    inline void SetRect(float x, float y, float w, float h);
    inline void SetPosition(float x, float y);
    inline void SetDimension(float w, float h);

    [[nodiscard]] inline auto GetAnchor() const -> const RectAnchor &;
    inline void SetAnchor(RectAnchor anchor);

    [[nodiscard]] inline auto GetScale() const -> float;
    inline void SetScale(float scale);

    inline auto GetTransformedRect() -> SDL_FRect;

    inline auto CanRender() -> bool;
    virtual inline void Render(SDL_Renderer *renderer);

    inline auto
    CheckCollisionAABB(const std::shared_ptr<RenderObject> &otherRenderObject)
        -> bool;

    inline auto CalculateBoundingBox() -> SDL_FRect;

    inline void DestroyChildObjects();

    [[nodiscard]] inline auto HasBeenMarkedForDestroy() const -> bool;

    inline void Destroy();
};

Game::Game() { Setup(); }

Game::~Game()
{
    children.clear();
    childrenBuffer.clear();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    ClearAudioCache();

    ClearFontCache();
    CleanupFontInits();

    ClearTextureCache();

    SDL_Quit();
};

void Game::AddChildObject(const std::shared_ptr<RenderObject> &child)
{
    child->game = this;

    children.emplace_back(child);
}

template <typename T>
auto Game::GetChildrenByType(const bool nested)
    -> std::vector<std::shared_ptr<T>>
{
    static_assert(std::is_base_of_v<RenderObject, T>,
                  "T must be derived from RenderObject");

    std::vector<std::shared_ptr<T>> results;

    for (const auto &child : childrenBuffer)
    {
        if (child == nullptr)
        {
            continue;
        }

        if (auto castedChild = std::dynamic_pointer_cast<T>(child))
        {
            results.emplace_back(castedChild);
        }

        if (nested)
        {
            auto childResults = child->GetChildrenByType<T>(nested);

            results.insert(results.end(), childResults.begin(),
                           childResults.end());
        }
    }

    return results;
}

template <typename T>
auto Game::GetChildByType(const bool nested) -> std::shared_ptr<T>
{
    static_assert(std::is_base_of_v<RenderObject, T>,
                  "T must be derived from RenderObject");

    if (auto childrenBuffer = GetChildrenByType<T>(nested);
        !childrenBuffer.empty())
    {
        return childrenBuffer.front();
    }

    return nullptr;
}

auto Game::GetChildCount() -> int { return children.size(); }

auto Game::GetWindow() -> SDL_Window * { return window; }

auto Game::GetRenderer() -> SDL_Renderer * { return renderer; }

auto Game::GetViewport() const -> const SDL_FRect & { return viewportf; }

auto Game::SwitchToFullscreen() -> bool
{
    return SDL_SetWindowFullscreen(window, SDL_TRUE) == SDL_TRUE;
}

auto Game::SwitchToWindowedMode() -> bool
{
    return SDL_SetWindowFullscreen(window, SDL_FALSE) == SDL_TRUE;
}

auto Game::Setup() -> bool
{
#ifdef _WIN32
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");
#endif

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return false;
    }

    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }

#ifdef __EMSCRIPTEN__
    window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
#else
    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
#endif

    SDL_SetWindowResizable(window, SDL_TRUE);

    if (window == nullptr)
    {
        SDL_Log("SDL_CreateWindow %s", SDL_GetError());

        return false;
    }

    if (renderer != nullptr)
    {
        SDL_DestroyRenderer(renderer);
    }

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr)
    {
        SDL_Log("SDL_CreateRenderer %s", SDL_GetError());

        return false;
    }

    SetScreenSize(width, height);

    return true;
}

void Game::SetScreenSize(const int _width, const int _height)
{
    SDL_SetWindowMinimumSize(window, _width, _height);

    SDL_SetWindowSize(window, _width, _height);

    width = _width;
    height = _height;

    viewport.w = width;
    viewport.h = height;
    viewportf.w = static_cast<float>(width);
    viewportf.h = static_cast<float>(height);

    SDL_RenderSetLogicalSize(renderer, width, height);
}

void Game::SetTitle(const char *name) { SDL_SetWindowTitle(window, name); }

void Game::SetClearColor(const SDL_Color color) { this->clearColor = color; }

auto Game::GetWidth() const -> int { return viewport.w; }

auto Game::GetHeight() const -> int { return viewport.h; }

auto Game::HasFocus() const -> bool { return focused; }

auto Game::IsKeyDown(const SDL_Keycode keyCode) const -> bool
{
    auto result = keyState.find(keyCode);

    if (result != keyState.end())
    {
        return result->second;
    }

    return false;
};

auto Game::IsAnyKeyPressed() const -> bool
{
    auto result = keyPressedState.size() > 0;

    return result;
};

auto Game::IsKeyPressed(const SDL_Keycode keyCode) const -> bool
{
    auto result = keyPressedState.find(keyCode);

    if (result != keyPressedState.end())
    {
        return result->second;
    }

    return false;
};

auto Game::IsKeyReleased(const SDL_Keycode keyCode) const -> bool
{
    auto result = keyReleasedState.find(keyCode);

    if (result != keyReleasedState.end())
    {
        return result->second;
    }

    return false;
};

auto Game::GetMousePosition() const -> SDL_FPoint { return mousePosition; };

auto Game::IsMouseButtonDown(const Uint8 buttonIndex) const -> bool
{
    auto result = mouseState.find(buttonIndex);

    if (result != mouseState.end())
    {
        return result->second;
    }

    return false;
};

auto Game::IsMouseButtonPressed(const Uint8 buttonIndex) const -> bool
{
    auto result = mousePressedState.find(buttonIndex);

    if (result != mousePressedState.end())
    {
        return result->second;
    }

    return false;
};

auto Game::IsMouseButtonReleased(const Uint8 buttonIndex) const -> bool
{
    auto result = mouseReleasedState.find(buttonIndex);

    if (result != mouseReleasedState.end())
    {
        return result->second;
    }

    return false;
};

auto Game::GetElapsedTime() const -> double { return elapsedTime; }

auto Game::GetFrameRate() const -> double { return frameRate; }

auto Game::GetFPS() const -> double { return fps; }

void Game::SetFrameRate(const double frameRate) { this->frameRate = frameRate; }

auto Game::GetQuit() const -> bool { return quit; }

auto Game::Run() -> int
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(Game::StaticLoop, this, 0, 1);
#else
    while (!GetQuit())
    {
        Loop();
    }
#endif

    return 0;
}

void Game::Loop()
{
    framesThisSecond++;

    auto frameStart = SDL_GetPerformanceCounter();

#ifdef __EMSCRIPTEN__
    if (GetQuit())
    {
        emscripten_cancel_main_loop();
    }
#endif

    HandleInput();

    FillChildrenBuffer();

    Update();
    FixedUpdate();

    Render();

    DestroyChildObjects();

    auto frameEnd = SDL_GetPerformanceCounter();

    deltaTime = (frameEnd - frameStart) /
                static_cast<double>(SDL_GetPerformanceFrequency());

#ifdef __EMSCRIPTEN__
    deltaTime = std::max(deltaTime, 0.01);
#endif

    float elapsedSeconds = (frameStart - previousFrameStart) /
                           (float)SDL_GetPerformanceFrequency();

    if (elapsedSeconds >= 1)
    {
        fps = (int)(framesThisSecond / elapsedSeconds);
        framesThisSecond = 0;
        previousFrameStart = frameStart;
    }

    SDL_Delay(1);
}

void Game::HandleInput()
{
    keyPressedState.clear();
    keyReleasedState.clear();

    mousePressedState.clear();
    mouseReleasedState.clear();

    while (SDL_PollEvent(&event) != 0)
    {
        auto keyCode = event.key.keysym.sym;

        auto mouseButtonIndex = event.button.button;

        switch (event.type)
        {
        case SDL_QUIT:
            Quit();
            break;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
            {
                focused = false;
            }
            else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
            {
                focused = true;
            }
            break;

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

        default:
            break;
        }
    }
}

void Game::FillChildrenBuffer()
{
    childrenBuffer = children;

    for (const auto &iter : childrenBuffer)
    {
        auto *child = iter.get();

        if (child != nullptr)
        {
            child->FillChildrenBuffer();
        }
    }
}

void Game::Update()
{
    elapsedTime += deltaTime;

    for (const auto &iter : childrenBuffer)
    {
        auto *child = iter.get();

        if (child != nullptr && child->IsEnabled())
        {
            child->InternalUpdate(deltaTime);
        }
    }
}

void Game::FixedUpdate()
{
    fixedUpdateDeltaTime += deltaTime;

    if (fixedUpdateDeltaTime > fixedFrameTime)
    {
        for (const auto &child : childrenBuffer)
        {
            if (child != nullptr && child->IsEnabled())
            {
                child->InternalFixedUpdate(fixedUpdateDeltaTime);
            }
        }

        fixedUpdateDeltaTime -= fixedFrameTime;
    }
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b,
                           clearColor.a);

    SDL_RenderClear(renderer);

    sort(childrenBuffer.begin(), childrenBuffer.end(),
         [](const std::shared_ptr<RenderObject> &a,
            const std::shared_ptr<RenderObject> &b) { return a->z < b->z; });

    for (const auto &child : childrenBuffer)
    {
        if (child != nullptr && child->IsEnabled())
        {
            child->Render(renderer);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::DestroyChildObjects()
{
    for (auto iter = children.begin(); iter != children.end();)
    {
        auto *child = iter->get();

        if (child != nullptr)
        {
            child->DestroyChildObjects();

            if (child->HasBeenMarkedForDestroy())
            {
                child->OnDestroy();

                iter->reset();
                iter = children.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
}

void Game::Quit() { quit = true; }

#ifdef HANDCRANK_ENGINE_DEBUG
void Game::ToggleDebug(const bool state) { debug = state; }
void Game::ToggleDebug() { debug = !debug; }
auto Game::IsDebug() const -> bool { return debug; }
#endif

RenderObject::RenderObject()
{
    SetPosition(0, 0);
    SetDimension(DEFAULT_RECT_WIDTH, DEFAULT_RECT_HEIGHT);

    index = ++RenderObject::count;
}

RenderObject::~RenderObject()
{
    children.clear();
    childrenBuffer.clear();
}

void RenderObject::Enable() { isEnabled = true; }

void RenderObject::Disable() { isEnabled = false; }

auto RenderObject::IsEnabled() const -> bool { return isEnabled; }

auto RenderObject::GetIndex() const -> int { return index; }

auto RenderObject::GetName() const -> std::string
{
    if (parent != nullptr)
    {
        return parent->GetName() + " > " + GetClassName() + " (" +
               std::to_string(index) + ")";
    }

    return GetClassName() + " (" + std::to_string(index) + ")";
}

auto RenderObject::GetClassName() const -> std::string
{
    return GetClassNameSimple(*this);
}

void RenderObject::AddChildObject(const std::shared_ptr<RenderObject> &child)
{
    if (game == nullptr)
    {
        throw std::runtime_error(
            "Parent render object has not been mounted yet.");
    }

    child->parent = this;

    child->game = game;

    children.emplace_back(child);
}

template <typename T>
auto RenderObject::GetChildrenByType(const bool nested)
    -> std::vector<std::shared_ptr<T>>
{
    static_assert(std::is_base_of_v<RenderObject, T>,
                  "T must be derived from RenderObject");

    std::vector<std::shared_ptr<T>> results;

    for (const auto &child : childrenBuffer)
    {
        if (child == nullptr)
        {
            continue;
        }

        if (auto castedChild = std::dynamic_pointer_cast<T>(child))
        {
            results.emplace_back(castedChild);
        }

        if (nested)
        {
            auto childResults = child->GetChildrenByType<T>(nested);

            results.insert(results.end(), childResults.begin(),
                           childResults.end());
        }
    }

    return results;
}

template <typename T>
auto RenderObject::GetChildByType(const bool nested) -> std::shared_ptr<T>
{
    static_assert(std::is_base_of_v<RenderObject, T>,
                  "T must be derived from RenderObject");

    auto children = GetChildrenByType<T>(nested);

    if (!children.empty())
    {
        return children.front();
    }

    return nullptr;
}

auto RenderObject::GetChildCount() -> int { return children.size(); }

void RenderObject::FillChildrenBuffer()
{
    childrenBuffer = children;

    for (const auto &iter : childrenBuffer)
    {
        auto *child = iter.get();

        if (child != nullptr)
        {
            child->FillChildrenBuffer();
        }
    }
}

void RenderObject::Start() {}

void RenderObject::Update(const double deltaTime) {}

void RenderObject::FixedUpdate(const double deltaTime) {}

void RenderObject::OnMouseOver() {}

void RenderObject::OnMouseOut() {}

void RenderObject::OnMouseDown() {}

void RenderObject::OnMouseUp() {}

void RenderObject::InternalUpdate(const double deltaTime)
{
    if (!hasStarted)
    {
        Start();

        hasStarted = true;
    }

    auto transformedRect = GetTransformedRect();

    auto mousePosition = game->GetMousePosition();

    if (SDL_PointInFRect(&mousePosition, &transformedRect) == SDL_TRUE)
    {
        if (game->IsMouseButtonPressed(SDL_BUTTON_LEFT))
        {
            OnMouseDown();

            isInputActive = true;
        }

        if (!isInputHovered)
        {
            OnMouseOver();

            isInputHovered = true;
        }
    }
    else if (isInputHovered)
    {
        OnMouseOut();

        isInputHovered = false;
    }

    if (isInputActive && game->IsMouseButtonReleased(SDL_BUTTON_LEFT))
    {
        OnMouseUp();

        isInputActive = false;
    }

    Update(deltaTime);

    for (const auto &child : childrenBuffer)
    {
        if (child != nullptr && child->IsEnabled())
        {
            child->InternalUpdate(deltaTime);
        }
    }
}

void RenderObject::InternalFixedUpdate(const double fixedDeltaTime)
{
    FixedUpdate(fixedDeltaTime);

    for (const auto &child : childrenBuffer)
    {
        if (child != nullptr && child->IsEnabled())
        {
            child->InternalFixedUpdate(fixedDeltaTime);
        }
    }
}

void RenderObject::OnDestroy() {}

auto RenderObject::GetRect() const -> const SDL_FRect & { return rect; }

void RenderObject::SetRect(const SDL_FRect &rect)
{
    SetPosition(rect.x, rect.y);
    SetDimension(rect.w, rect.h);
}

void RenderObject::SetRect(const float x, const float y, const float w,
                           const float h)
{
    SetPosition(x, y);
    SetDimension(w, h);
}

void RenderObject::SetPosition(const float x, const float y)
{
    rect.x = x;
    rect.y = y;
}

void RenderObject::SetDimension(const float w, const float h)
{
    rect.w = w;
    rect.h = h;
}

auto RenderObject::GetAnchor() const -> const RectAnchor & { return anchor; }
void RenderObject::SetAnchor(const RectAnchor anchor) { this->anchor = anchor; }

auto RenderObject::GetScale() const -> float { return scale; }

void RenderObject::SetScale(const float scale) { this->scale = scale; }

auto RenderObject::GetTransformedRect() -> SDL_FRect
{
    SDL_FRect transformedRect = {rect.x, rect.y, rect.w, rect.h};

    transformedRect.w *= scale;
    transformedRect.h *= scale;

    if ((anchor & RectAnchor::HCENTER) == RectAnchor::HCENTER)
    {
        transformedRect.x -= transformedRect.w / 2;
    }
    else if ((anchor & RectAnchor::RIGHT) == RectAnchor::RIGHT)
    {
        transformedRect.x -= transformedRect.w;
    }

    if ((anchor & RectAnchor::VCENTER) == RectAnchor::VCENTER)
    {
        transformedRect.y -= transformedRect.h / 2;
    }
    else if ((anchor & RectAnchor::BOTTOM) == RectAnchor::BOTTOM)
    {
        transformedRect.y -= transformedRect.h;
    }

    if (parent != nullptr)
    {
        transformedRect.x += parent->GetRect().x;
        transformedRect.y += parent->GetRect().y;

        transformedRect.w *= parent->scale;
        transformedRect.h *= parent->scale;
    }

    return transformedRect;
}

auto RenderObject::CanRender() -> bool
{
    auto boundingBox = CalculateBoundingBox();

    auto viewport = game->GetViewport();

    return SDL_HasIntersectionF(&boundingBox, &viewport) == SDL_TRUE;
}

void RenderObject::Render(SDL_Renderer *renderer)
{
    if (!CanRender())
    {
        return;
    }

    sort(childrenBuffer.begin(), childrenBuffer.end(),
         [](const std::shared_ptr<RenderObject> &a,
            const std::shared_ptr<RenderObject> &b) { return a->z < b->z; });

    for (const auto &child : childrenBuffer)
    {
        if (child != nullptr && child->IsEnabled())
        {
            child->Render(renderer);
        }
    }

#ifdef HANDCRANK_ENGINE_DEBUG
    if (game->IsDebug())
    {
        auto transformedRect = GetTransformedRect();

        if (debugRectTexture == nullptr)
        {
            auto *tempSurface = SDL_CreateRGBSurfaceWithFormat(
                0, 1, 1, 32, SDL_PIXELFORMAT_RGBA32);

            if (tempSurface != nullptr)
            {
                SDL_FillRect(tempSurface, nullptr,
                             SDL_MapRGBA(tempSurface->format, 0, 255, 0, 100));

                debugRectTexture = std::shared_ptr<SDL_Texture>(
                    SDL_CreateTextureFromSurface(renderer, tempSurface),
                    SDL_DestroyTexture);

                SDL_FreeSurface(tempSurface);
            }
        }

        SDL_RenderCopyF(renderer, debugRectTexture.get(), nullptr,
                        &transformedRect);
    }
#endif
}

[[nodiscard]] auto RenderObject::CalculateBoundingBox() -> SDL_FRect
{
    auto boundingBox = GetTransformedRect();

    for (const auto &child : childrenBuffer)
    {
        if (child->IsEnabled())
        {
            const auto childBoundingBox = child->CalculateBoundingBox();

            float rightExtent = childBoundingBox.x + childBoundingBox.w;
            float bottomExtent = childBoundingBox.y + childBoundingBox.h;
            float currentRight = boundingBox.x + boundingBox.w;
            float currentBottom = boundingBox.y + boundingBox.h;

            boundingBox.x = fminf(boundingBox.x, childBoundingBox.x);
            boundingBox.y = fminf(boundingBox.y, childBoundingBox.y);

            boundingBox.w = fmaxf(currentRight, rightExtent) - boundingBox.x;
            boundingBox.h = fmaxf(currentBottom, bottomExtent) - boundingBox.y;
        }
    }

    return boundingBox;
}

auto RenderObject::CheckCollisionAABB(
    const std::shared_ptr<RenderObject> &otherRenderObject) -> bool
{
    auto thisRect = GetTransformedRect();
    auto otherRect = otherRenderObject->GetTransformedRect();
    return SDL_HasIntersectionF(&thisRect, &otherRect) == SDL_TRUE;
}

void RenderObject::DestroyChildObjects()
{
    for (auto iter = children.begin(); iter != children.end();)
    {
        auto *child = iter->get();

        if (child != nullptr)
        {
            child->DestroyChildObjects();

            if (child->HasBeenMarkedForDestroy())
            {
                child->OnDestroy();

                iter->reset();
                iter = children.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
}

[[nodiscard]] auto RenderObject::HasBeenMarkedForDestroy() const -> bool
{
    return isMarkedForDestroy;
}

void RenderObject::Destroy()
{
    isMarkedForDestroy = true;

    for (const auto &child : children)
    {
        if (child != nullptr)
        {
            child->Destroy();
        }
    }
}

} // namespace HandcrankEngine
