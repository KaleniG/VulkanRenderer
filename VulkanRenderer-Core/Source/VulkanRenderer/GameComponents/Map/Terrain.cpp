#include <vkrenpch.h>

#include "VulkanRenderer/GameComponents/Map/Terrain.h"
#include "VulkanRenderer/Renderer/Vertex.h"

#define VKREN_TILE_VERTEX_COUNT 6
#define VKREN_TILE_FEET_TO_METERS_HEIGHT 0.65f * 0.3048f

namespace vkren
{
  Terrain::Terrain(glm::uvec2 size, uint32_t start_height, const Ref<Texture>& default_texture)
    : m_Heightmap(size + glm::uvec2(1, 1), start_height * VKREN_TILE_FEET_TO_METERS_HEIGHT), r_DefaultTexture(default_texture) {}

  Terrain::Terrain(const Heightmap& heightmap, const Ref<Texture>& default_texture)
    : m_Heightmap(heightmap), r_DefaultTexture(default_texture) {}

  void Terrain::RaiseTerrain(glm::uvec2 position, float raise_range, float feather_range, float amount)
  {
    const glm::vec2 center = glm::vec2(position);
    const float max_range = raise_range + feather_range;

    for (uint32_t x = 0; x < m_Heightmap.GetSize().x; ++x)
    {
      for (uint32_t y = 0; y < m_Heightmap.GetSize().y; ++y)
      {
        glm::vec2 current = glm::vec2(x, y);
        float distance = glm::distance(current, center);

        if (distance <= max_range)
        {
          float influence = 0.0f;

          if (distance <= raise_range)
            influence = 1.0f;
          else if (distance <= max_range)
            influence = (max_range - distance) / feather_range;

          float currentHeight = m_Heightmap.GetHeight(x, y);
          m_Heightmap.SetHeight(x, y, currentHeight + amount * influence);
        }
      }
    }

    m_Saved = false;
  }

  const Ref<VertexBuffer>& Terrain::GetVertexBuffer()
  {
    if (!m_Saved)
    {
      Terrain::CalculateVerticesAndIndices();
      m_Saved = true;
    }
    return m_VertexBuffer;
  }

  const Ref<IndexBuffer>& Terrain::GetIndexBuffer()
  {
    if (!m_Saved)
    {
      Terrain::CalculateVerticesAndIndices();
      m_Saved = true;
    }
    return m_IndexBuffer;
  }

  void Terrain::ExpandHeightmap(DirectionMask direction, glm::uvec2::value_type amount)
  {
    m_Heightmap.Expand(direction, amount);
    m_Saved = false;
  }

  void Terrain::ResizeHeightmap(glm::uvec2 size)
  {
    m_Heightmap.Resize(size);
    m_Saved = false;
  }

  void Terrain::CalculateVerticesAndIndices()
  {
    std::vector<TerrainVertex> vertexData;
    std::vector<uint32_t> indexData;

    std::unordered_map<TerrainVertex, uint32_t> uniqueVertices;

    for (uint32_t x = 0; x < m_Heightmap.GetSize().x - 1; x++)
    {
      for (uint32_t y = 0; y < m_Heightmap.GetSize().y - 1; y++)
      {
        const glm::vec3 basePosition = glm::vec3(x * 10.0f, y * 10.0f, 0.0f);
        constexpr float halfSize = 5.0f;

        std::array<float, 4> tileHeights;
        tileHeights[0] = m_Heightmap.GetHeight(x, y);         // BOTTOM LEFT
        tileHeights[1] = m_Heightmap.GetHeight(x + 1, y);     // BOTTOM RIGHT
        tileHeights[2] = m_Heightmap.GetHeight(x, y + 1);     // TOP LEFT
        tileHeights[3] = m_Heightmap.GetHeight(x + 1, y + 1); // TOP RIGHT

        bool rotateTile = true;

        auto maxIter = std::max_element(tileHeights.begin(), tileHeights.end());

        size_t maxIndex = std::distance(tileHeights.begin(), maxIter);
        if (maxIndex == 3 || maxIndex == 0)
          rotateTile = false;

        float maxHeight = *maxIter;
        for (float height : tileHeights)
        {
          if (height != maxHeight && maxHeight - height < 24.0f)
          {
            rotateTile = false;
            break;
          }
        }

        std::array<TerrainVertex, VKREN_TILE_VERTEX_COUNT> vertices;
        if (rotateTile)
        {
          vertices[0].Position = glm::vec3(halfSize, -halfSize, tileHeights[1] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // BOTTOM RIGHT
          vertices[0].TextureCoord = glm::vec2(1.0f, 1.0f);
          vertices[1].Position = glm::vec3(halfSize, halfSize, tileHeights[3] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // TOP RIGHT
          vertices[1].TextureCoord = glm::vec2(1.0f, 0.0f);
          vertices[2].Position = glm::vec3(-halfSize, -halfSize, tileHeights[0] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // BOTTOM LEFT
          vertices[2].TextureCoord = glm::vec2(0.0f, 1.0f);
          vertices[3].Position = glm::vec3(-halfSize, -halfSize, tileHeights[0] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // BOTTOM LEFT
          vertices[3].TextureCoord = glm::vec2(0.0f, 1.0f);
          vertices[4].Position = glm::vec3(halfSize, halfSize, tileHeights[3] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // TOP RIGHT
          vertices[4].TextureCoord = glm::vec2(1.0f, 0.0f);
          vertices[5].Position = glm::vec3(-halfSize, halfSize, tileHeights[2] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // TOP LEFT
          vertices[5].TextureCoord = glm::vec2(0.0f, 0.0f);
        }
        else
        {
          vertices[0].Position = glm::vec3(-halfSize, -halfSize, tileHeights[0] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // BOTTOM LEFT
          vertices[0].TextureCoord = glm::vec2(0.0f, 1.0f);
          vertices[1].Position = glm::vec3(halfSize, -halfSize, tileHeights[1] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // BOTTOM RIGHT
          vertices[1].TextureCoord = glm::vec2(1.0f, 1.0f);
          vertices[2].Position = glm::vec3(-halfSize, halfSize, tileHeights[2] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // TOP LEFT
          vertices[2].TextureCoord = glm::vec2(0.0f, 0.0f);
          vertices[3].Position = glm::vec3(halfSize, -halfSize, tileHeights[1] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // BOTTOM RIGHT
          vertices[3].TextureCoord = glm::vec2(1.0f, 1.0f);
          vertices[4].Position = glm::vec3(halfSize, halfSize, tileHeights[3] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // TOP RIGHT
          vertices[4].TextureCoord = glm::vec2(1.0f, 0.0f);
          vertices[5].Position = glm::vec3(-halfSize, halfSize, tileHeights[2] * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition; // TOP LEFT
          vertices[5].TextureCoord = glm::vec2(0.0f, 0.0f);
        }

        for (TerrainVertex& tileVertex : vertices)
        {
          tileVertex.Color = glm::vec3(1.0f, 1.0f, 1.0f);

          if (uniqueVertices.count(tileVertex) == 0)
          {
            uniqueVertices[tileVertex] = static_cast<uint32_t>(vertexData.size());
            vertexData.push_back(tileVertex);
          }

          indexData.emplace_back(uniqueVertices[tileVertex]);
        }
      }
    }

    m_VertexBuffer = VertexBuffer::Create(vertexData);
    m_IndexBuffer = IndexBuffer::Create(indexData);

    m_Saved = true;
  }

}