#pragma once

#include <glm/glm.hpp>

#include "VulkanRenderer/GameComponents/Map/Tile.h"
#include "VulkanRenderer/Renderer/Resources/Texture.h"
#include "VulkanRenderer/Math/Hashes.h"

namespace vkren
{

  struct Vertex;
  class Terrain
  {
  public:
    Terrain() = default;
    Terrain(glm::uvec2 size, uint32_t start_height, const Ref<Texture>& default_texture);

    void RaiseTerrain(glm::ivec2 position, int32_t raise_range, int32_t feather_range, uint32_t amount);

    const std::vector<Vertex>& GetVertices();
    const std::vector<uint32_t>& GetIndices();

  private:
    void CalculateVerticesAndIndices();
    Tile* FindTile(glm::ivec2 position);
    void AdjustSurroundingTilesTo(Tile* tile, glm::ivec2 position);

  private:
    glm::uvec2 m_Size;
    std::unordered_map<glm::ivec2, Tile, glm::ivec2Hash> m_Tiles;
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;

    bool m_Saved = false;
  };

}