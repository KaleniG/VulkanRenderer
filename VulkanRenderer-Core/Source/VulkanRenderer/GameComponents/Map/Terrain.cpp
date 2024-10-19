#include <vkrenpch.h>

#include "VulkanRenderer/GameComponents/Map/Terrain.h"

#define VKREN_TILE_VERTEX_COUNT 6

namespace vkren
{

  Terrain::Terrain(glm::ivec2 size, uint32_t start_height, const Ref<Texture>& default_texture)
    : m_Size(size)
  {
    m_Tiles.reserve(size.x * size.y);

    for (int x = 0; x < size.x; x++)
      for (int y = 0; y < size.x; y++)
        m_Tiles.emplace_back(glm::ivec2(x, y), start_height, default_texture, true);

    Terrain::CalculateVerticesAndIndices();
  }

  void Terrain::CalculateVerticesAndIndices()
  {
    m_Vertices.clear();
    m_Indices.clear();
    m_Indices.shrink_to_fit();
    m_Indices.reserve(m_Tiles.size() * 6);

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const Tile& tile : m_Tiles)
    {
      const glm::vec3 basePosition = glm::vec3(tile.GetPosition().x * 10.0f, tile.GetPosition().y * 10.0f, 0.0f);
      const float halfSize = 5.0f;

      std::array<Vertex, 6> vertices;
      vertices[0].Position = glm::vec3(halfSize, -halfSize, static_cast<float>(tile.GetHeight(Edge::BottomRight))) + basePosition;
      vertices[0].TextureCoord = glm::vec2(1.0f, 1.0f);
      vertices[1].Position = glm::vec3(halfSize, halfSize, static_cast<float>(tile.GetHeight(Edge::TopRight))) + basePosition;
      vertices[1].TextureCoord = glm::vec2(1.0f, 0.0f);
      vertices[2].Position = glm::vec3(-halfSize, -halfSize, static_cast<float>(tile.GetHeight(Edge::BottomLeft))) + basePosition;
      vertices[2].TextureCoord = glm::vec2(0.0f, 1.0f);
      vertices[3].Position = glm::vec3(halfSize, halfSize, static_cast<float>(tile.GetHeight(Edge::TopRight))) + basePosition;
      vertices[3].TextureCoord = glm::vec2(1.0f, 0.0f);
      vertices[4].Position = glm::vec3(-halfSize, halfSize, static_cast<float>(tile.GetHeight(Edge::TopLeft))) + basePosition;
      vertices[4].TextureCoord = glm::vec2(0.0f, 0.0f);
      vertices[5].Position = glm::vec3(-halfSize, -halfSize, static_cast<float>(tile.GetHeight(Edge::BottomLeft))) + basePosition;
      vertices[5].TextureCoord = glm::vec2(0.0f, 1.0f);

      for (Vertex& tileVertex : vertices)
      {
        tileVertex.Color = tile.IsPassable() ? glm::vec3(1.0f) : glm::vec3(1.0f, 0.0f, 0.0f);

        if (uniqueVertices.count(tileVertex) == 0)
        {
          uniqueVertices[tileVertex] = static_cast<uint32_t>(m_Vertices.size());
          m_Vertices.push_back(tileVertex);
        }

        m_Indices.push_back(uniqueVertices[tileVertex]);
      }
    }
  }

}