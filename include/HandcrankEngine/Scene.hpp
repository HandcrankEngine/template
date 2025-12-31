// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <typeindex>

#include "HandcrankEngine/HandcrankEngine.hpp"

namespace HandcrankEngine
{

class Scene : public RenderObject
{
  private:
    std::function<void(std::type_index)> SetCurrentScene;

  public:
    using RenderObject::RenderObject;

    void SetCurrentSceneCallback(
        const std::function<void(std::type_index)> &callback)
    {
        SetCurrentScene = callback;
    }

    template <typename T> void SwitchToScene()
    {
        static_assert(std::is_base_of_v<Scene, T>,
                      "T must be derive from Scene");

        if (SetCurrentScene)
        {
            SetCurrentScene(std::type_index(typeid(T)));
        }
    }
};

} // namespace HandcrankEngine
