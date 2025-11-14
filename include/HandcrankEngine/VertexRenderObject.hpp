// Copyright (c) Scott Doxey. All Rights Reserved. Licensed under the MIT
// License. See LICENSE in the project root for license information.

#pragma once

#include <SDL.h>

#include "HandcrankEngine.hpp"
#include "TextureRenderObject.hpp"

namespace HandcrankEngine
{

class VertexRenderObject : public TextureRenderObject
{
  protected:
    std::vector<SDL_Vertex> vertices;
    std::vector<int> indices;

  public:
    explicit VertexRenderObject() = default;

    ~VertexRenderObject() override = default;

    void Render(SDL_Renderer *renderer) override
    {
        SDL_RenderGeometry(game->GetRenderer(), texture, vertices.data(),
                           vertices.size(), indices.data(), indices.size());

        RenderObject::Render(renderer);
    }
};

} // namespace HandcrankEngine
