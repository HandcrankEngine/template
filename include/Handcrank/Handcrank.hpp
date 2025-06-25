// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

// #define HANDCRANK_DEBUG 1

#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define HANDCRANK_VERSION_MAJOR 0
#define HANDCRANK_VERSION_MINOR 0
#define HANDCRANK_VERSION_PATCH 0

#include <functional>
#include <iostream>
#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Utilities.hpp"

namespace Handcrank
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

std::shared_ptr<SDL_Texture> debugRectTexture;

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

    Uint64 previousTime;

    double deltaTime = 0;
    double renderDeltaTime = 0;
    double fixedUpdateDeltaTime = 0;

    double frameRate = DEFAULT_FRAME_RATE;

#ifdef __EMSCRIPTEN__
    bool capFrameRate = false;
#else
    bool capFrameRate = true;
#endif

    double fps = 0;

    double targetFrameTime = 1.0 / frameRate;

    const double fixedFrameTime = 0.02;

    int width = DEFAULT_WINDOW_WIDTH;
    int height = DEFAULT_WINDOW_HEIGHT;

    float dpiScaleX = 1;
    float dpiScaleY = 1;

    bool focused = false;

#ifdef HANDCRANK_DEBUG
    bool debug = false;
#endif

  public:
    std::unordered_map<SDL_Keycode, bool> keyState;
    std::unordered_map<SDL_Keycode, bool> keyPressedState;
    std::unordered_map<SDL_Keycode, bool> keyReleasedState;

    SDL_FPoint mousePosition{};

    std::unordered_map<Uint8, bool> mouseState;
    std::unordered_map<Uint8, bool> mousePressedState;
    std::unordered_map<Uint8, bool> mouseReleasedState;

    std::vector<std::shared_ptr<RenderObject>> children;

    inline Game();
    inline ~Game();

    inline void AddChildObject(const std::shared_ptr<RenderObject> &child);

    template <typename T>
    inline auto GetChildrenByType(bool nested = false)
        -> std::vector<std::shared_ptr<T>>;
    template <typename T>
    inline auto GetChildByType(bool nested = false) -> std::shared_ptr<T>;

    [[nodiscard]] inline auto GetWindow() -> SDL_Window *;
    [[nodiscard]] inline auto GetRenderer() -> SDL_Renderer *;
    [[nodiscard]] inline auto GetViewport() const -> const SDL_FRect &;

    inline auto Setup() -> bool;

    inline void SetScreenSize(int _width, int _height);

    inline void SetTitle(const char *name);

    inline void SetClearColor(SDL_Color color);

    [[nodiscard]] inline auto GetWidth() const -> int;
    [[nodiscard]] inline auto GetHeight() const -> int;

    [[nodiscard]] inline auto HasFocus() const -> bool;

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

    inline void Update();
    inline void FixedUpdate();

    inline void Render();

    inline void DestroyChildObjects();

    inline void Quit();

#ifdef HANDCRANK_DEBUG
    inline void ToggleDebug(bool state);
    inline void ToggleDebug();
    [[nodiscard]] inline auto IsDebug() const -> bool;
#endif
};

class RenderObject : public std::enable_shared_from_this<RenderObject>
{
  protected:
    static unsigned int count;

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

    std::function<void(RenderObject *)> startFunction;

    std::function<void(RenderObject *, double)> updateFunction;

    std::function<void(RenderObject *, double)> fixedUpdateFunction;

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

    inline ~RenderObject();

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

    inline void SetStart(std::function<void(RenderObject *)> func);
    inline void SetUpdate(std::function<void(RenderObject *, double)> func);
    inline void
    SetFixedUpdate(std::function<void(RenderObject *, double)> func);

    virtual inline void Start();
    virtual inline void Update(double deltaTime);
    virtual inline void FixedUpdate(double deltaTime);

    virtual inline void OnMouseOver();
    virtual inline void OnMouseOut();
    virtual inline void OnMouseDown();
    virtual inline void OnMouseUp();

    virtual inline void InternalUpdate(double deltaTime);
    virtual inline void InternalFixedUpdate(double fixedDeltaTime);

    [[nodiscard]] inline auto GetRect() const -> const SDL_FRect &;
    inline void SetRect(SDL_FRect rect);
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

Game::Game()
{
    Setup();

    previousTime = 0;
}

Game::~Game()
{
    for (auto child : children)
    {
        child.reset();
    }

    children.clear();

    for (auto i = 0; i < TTF_WasInit(); i += 1)
    {
        TTF_Quit();
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
};

void Game::AddChildObject(const std::shared_ptr<RenderObject> &child)
{
    child->game = this;

    children.push_back(child);
}

template <typename T>
auto Game::GetChildrenByType(const bool nested)
    -> std::vector<std::shared_ptr<T>>
{
    static_assert(std::is_base_of_v<RenderObject, T>,
                  "T must be derived from RenderObject");

    std::vector<std::shared_ptr<T>> results;

    for (const auto &child : children)
    {
        if (auto castedChild = std::dynamic_pointer_cast<T>(child))
        {
            results.push_back(castedChild);
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

    if (auto children = GetChildrenByType<T>(nested); !children.empty())
    {
        return children.front();
    }

    return nullptr;
}

auto Game::GetWindow() -> SDL_Window * { return window; }

auto Game::GetRenderer() -> SDL_Renderer * { return renderer; }

auto Game::GetViewport() const -> const SDL_FRect & { return viewportf; }

auto Game::Setup() -> bool
{
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "1");

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return false;
    }

    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }

    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

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
    SDL_SetWindowSize(window, _width, _height);

    SDL_GL_GetDrawableSize(window, &width, &height);

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);

    viewport.w = width;
    viewport.h = height;

    viewportf.w = static_cast<float>(width);
    viewportf.h = static_cast<float>(height);

    dpiScaleX = (float)width / _width;
    dpiScaleY = (float)height / _height;
}

void Game::SetTitle(const char *name) { SDL_SetWindowTitle(window, name); }

void Game::SetClearColor(const SDL_Color color) { this->clearColor = color; }

auto Game::GetWidth() const -> int { return width; }

auto Game::GetHeight() const -> int { return height; }

auto Game::HasFocus() const -> bool { return focused; }

auto Game::GetFrameRate() const -> double { return frameRate; }

auto Game::GetFPS() const -> double { return fps; }

void Game::SetFrameRate(const double frameRate)
{
    this->frameRate = frameRate;

    this->targetFrameTime = 1.0 / frameRate;
}

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

#ifdef __EMSCRIPTEN__
    if (GetQuit())
    {
        emscripten_cancel_main_loop();
    }
#endif

    HandleInput();

    CalculateDeltaTime();

    Update();
    FixedUpdate();

    Render();

    DestroyChildObjects();
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
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_RESTORED ||
                event.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
                event.window.event == SDL_WINDOWEVENT_MINIMIZED)
            {
                SDL_GL_GetDrawableSize(window, &width, &height);
            }
            else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
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
            keyPressedState[keyCode] = false;
            keyReleasedState[keyCode] = true;
            break;

        case SDL_MOUSEMOTION:
            mousePosition.x = event.motion.x * dpiScaleX;
            mousePosition.y = event.motion.y * dpiScaleY;
            break;

        case SDL_MOUSEBUTTONDOWN:
            mousePressedState[mouseButtonIndex] = !mouseState[mouseButtonIndex];
            mouseState[mouseButtonIndex] = true;
            break;

        case SDL_MOUSEBUTTONUP:
            mouseState[mouseButtonIndex] = false;
            mousePressedState[mouseButtonIndex] = false;
            mouseReleasedState[mouseButtonIndex] = true;
            break;

        default:
            break;
        }
    }
}

void Game::CalculateDeltaTime()
{
    auto currentTime = SDL_GetPerformanceCounter();

    if (previousTime == 0)
    {
        deltaTime = fixedFrameTime;
    }
    else
    {
        deltaTime = (currentTime - previousTime) /
                    static_cast<double>(SDL_GetPerformanceFrequency());
    }

    previousTime = currentTime;
}

void Game::Update()
{
    for (const auto &iter : children)
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
        for (const auto &child : children)
        {
            if (child->IsEnabled())
            {
                child->InternalFixedUpdate(fixedUpdateDeltaTime);
            }
        }

        fixedUpdateDeltaTime -= fixedFrameTime;
    }
}

void Game::Render()
{
    renderDeltaTime += deltaTime;

    if (renderDeltaTime > targetFrameTime || !capFrameRate)
    {
        SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g,
                               clearColor.b, clearColor.a);

        SDL_RenderClear(renderer);

        SDL_RenderSetViewport(renderer, &viewport);

        sort(children.begin(), children.end(),
             [](const std::shared_ptr<RenderObject> &a,
                const std::shared_ptr<RenderObject> &b)
             { return a->z < b->z; });

        for (const auto &child : children)
        {
            if (child->IsEnabled())
            {
                child->Render(renderer);
            }
        }

        SDL_RenderPresent(renderer);

        fps = 1.0 / renderDeltaTime;

        renderDeltaTime = 0;
    }
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

#ifdef HANDCRANK_DEBUG
void Game::ToggleDebug(const bool state) { debug = state; }
void Game::ToggleDebug() { debug = !debug; }
auto Game::IsDebug() const -> bool { return debug; }
#endif

unsigned int RenderObject::count = 0;

RenderObject::RenderObject()
{
    SetPosition(0, 0);
    SetDimension(DEFAULT_RECT_WIDTH, DEFAULT_RECT_HEIGHT);

    index = ++RenderObject::count;
}

RenderObject::~RenderObject()
{
    for (auto child : children)
    {
        child.reset();
    }

    children.clear();
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
    child->parent = this;

    child->game = game;

    children.push_back(child);
}

template <typename T>
auto RenderObject::GetChildrenByType(const bool nested)
    -> std::vector<std::shared_ptr<T>>
{
    static_assert(std::is_base_of_v<RenderObject, T>,
                  "T must be derived from RenderObject");

    std::vector<std::shared_ptr<T>> results;

    for (const auto &child : children)
    {
        if (auto castedChild = std::dynamic_pointer_cast<T>(child))
        {
            results.push_back(castedChild);
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

void RenderObject::SetStart(std::function<void(RenderObject *)> func)
{
    if (startFunction)
    {
        std::cerr << "WARNING! Start function has already been set. "
                     "Overriding with new function.\n";
    }

    startFunction = std::move(func);
}

void RenderObject::SetUpdate(std::function<void(RenderObject *, double)> func)
{
    if (updateFunction)
    {
        std::cerr << "WARNING! Update function has already been set. "
                     "Overriding with new function.\n";
    }

    updateFunction = std::move(func);
}

void RenderObject::SetFixedUpdate(
    std::function<void(RenderObject *, double)> func)
{
    if (fixedUpdateFunction)
    {
        std::cerr << "WARNING! Fixed update function has already been set. "
                     "Overriding with new function.\n";
    }

    fixedUpdateFunction = std::move(func);
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

        if (startFunction)
        {
            startFunction(this);
        }

        hasStarted = true;
    }

    auto transformedRect = GetTransformedRect();

    if (SDL_PointInFRect(&game->mousePosition, &transformedRect) == SDL_TRUE)
    {
        if (game->mousePressedState[SDL_BUTTON_LEFT])
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

    if (isInputActive && game->mouseReleasedState[SDL_BUTTON_LEFT])
    {
        OnMouseUp();

        isInputActive = false;
    }

    Update(deltaTime);

    if (updateFunction)
    {
        updateFunction(this, deltaTime);
    }

    for (const auto &child : children)
    {
        if (child->IsEnabled())
        {
            child->InternalUpdate(deltaTime);
        }
    }
}

void RenderObject::InternalFixedUpdate(const double fixedDeltaTime)
{
    FixedUpdate(fixedDeltaTime);

    if (fixedUpdateFunction)
    {
        fixedUpdateFunction(this, fixedDeltaTime);
    }

    for (const auto &child : children)
    {
        if (child->IsEnabled())
        {
            child->InternalFixedUpdate(fixedDeltaTime);
        }
    }
}

auto RenderObject::GetRect() const -> const SDL_FRect & { return rect; }

void RenderObject::SetRect(const SDL_FRect rect)
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

    sort(children.begin(), children.end(),
         [](const std::shared_ptr<RenderObject> &a,
            const std::shared_ptr<RenderObject> &b) { return a->z < b->z; });

    for (const auto &child : children)
    {
        if (child->IsEnabled())
        {
            child->Render(renderer);
        }
    }

#ifdef HANDCRANK_DEBUG
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

    for (const auto &child : children)
    {
        if (child->IsEnabled() && child->CanRender())
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
    return SDL_HasIntersectionF(&rect, &otherRenderObject->GetRect()) ==
           SDL_TRUE;
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
        child->Destroy();
    }
}

} // namespace Handcrank
