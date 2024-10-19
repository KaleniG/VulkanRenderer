#pragma once

#include <glm/glm.hpp>

#include "VulkanRenderer/GameComponents/Map/Tile.h"
#include "VulkanRenderer/Renderer/Resources/Texture.h"

namespace vkren
{

  struct Vertex;
  class Terrain
  {
  public:
    Terrain() = default;
    Terrain(glm::uvec2 size, uint32_t start_height, const Ref<Texture>& default_texture);

    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

  private:
    void CalculateVerticesAndIndices();

  private:
    glm::uvec2 m_Size;
    std::vector<Tile> m_Tiles;
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
  };

}