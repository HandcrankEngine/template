// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <cstddef>
#include <memory>

#include "HandcrankEngine/HandcrankEngine.hpp"
#include "HandcrankEngine/Scene.hpp"

namespace HandcrankEngine
{

class SceneManager : public RenderObject
{

  private:
    std::vector<std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> currentScene;

  public:
    using RenderObject::RenderObject;

    void Start() override {}

    auto SetCurrentScene(const std::shared_ptr<Scene> &scene) -> bool
    {
        if (currentScene == scene)
        {
            return false;
        }

        if (currentScene != nullptr)
        {
            currentScene->Destroy();
        }

        currentScene = scene;

        if (currentScene != nullptr)
        {
            AddChildObject(currentScene);

            return true;
        }

        return false;
    }

    template <typename T> auto SetCurrentScene() -> bool
    {
        static_assert(std::is_base_of_v<Scene, T>,
                      "T must be derive from Scene");

        auto it =
            std::find_if(scenes.begin(), scenes.end(),
                         [](const std::shared_ptr<Scene> &scene)
                         { return dynamic_cast<T *>(scene.get()) != nullptr; });

        if (it != scenes.end())
        {
            return SetCurrentScene(*it);
        }

        return false;
    }

    template <typename T> auto GetCurrentScene() -> std::shared_ptr<T>
    {
        static_assert(std::is_base_of_v<Scene, T>,
                      "T must be derive from Scene");

        return std::dynamic_pointer_cast<T>(currentScene);
    }

    auto GetCurrentScene() -> std::shared_ptr<Scene> { return currentScene; }

    void AddScene(std::shared_ptr<Scene> scene)
    {
        if (std::find(scenes.begin(), scenes.end(), scene) == scenes.end())
        {
            scene->SetCurrentSceneCallback(
                [this](std::type_index typeIndex) -> void
                {
                    auto it = std::find_if(
                        scenes.begin(), scenes.end(),
                        [typeIndex](const std::shared_ptr<Scene> &scene)
                        {
                            if (!scene)
                            {
                                return false;
                            }

                            const Scene &sceneRef = *scene;

                            return std::type_index(typeid(sceneRef)) ==
                                   typeIndex;
                        });

                    if (it != scenes.end())
                    {
                        SetCurrentScene(*it);
                    }
                });
            scenes.emplace_back(scene);
        }
    }

    void Update(double deltaTime) override
    {
        if (!game->HasFocus())
        {
            return;
        }

        SetupCurrentScene();
        CleanupCurrentScene();
    }

  private:
    void SetupCurrentScene() {}

    void CleanupCurrentScene() {}
};

} // namespace HandcrankEngine
