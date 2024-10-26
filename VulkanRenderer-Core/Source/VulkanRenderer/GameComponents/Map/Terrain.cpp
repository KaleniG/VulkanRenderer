#include <vkrenpch.h>

#include "VulkanRenderer/GameComponents/Map/Terrain.h"
#include "VulkanRenderer/Renderer/Vertex.h"

#define VKREN_TILE_VERTEX_COUNT 6
#define VKREN_TILE_FEET_TO_METERS_HEIGHT 0.65f * 0.3048f

namespace vkren
{

  Terrain::Terrain(glm::uvec2 size, uint32_t start_height, const Ref<Texture>& default_texture)
    : m_Size(size)
  {
    for (int x = 0; x < size.x; x++)
      for (int y = 0; y < size.y; y++)
        m_Tiles.emplace(glm::ivec2(x, y), Tile(start_height, default_texture, true));

    Terrain::CalculateVerticesAndIndices();
  }

  void Terrain::RaiseTerrain(glm::ivec2 position, int32_t raise_range, int32_t feather_range, uint32_t amount)
  {
    feather_range += raise_range;

    struct FeatherTile
    {
      Tile* tile;
      float distance;
    };

    std::unordered_map<glm::ivec2, Tile*> tilesInRange;
    std::unordered_map<glm::ivec2, FeatherTile> tilesInFeatherRange;

    const float raiseRangeSquared = raise_range * raise_range;
    const float featherRangeSquared = feather_range * feather_range;

    const int32_t raiseFeatherRange = raise_range + feather_range;

    for (int x = -raiseFeatherRange; x <= raiseFeatherRange; x++)
    {
      for (int y = -raiseFeatherRange; y <= raiseFeatherRange; y++)
      {
        Tile* tile = Terrain::FindTile(position + glm::ivec2(x, y));
        if (tile)
        {
          float distanceSquared = static_cast<float>(x * x + y * y);

          if (distanceSquared <= raiseRangeSquared)
            tilesInRange.emplace(position + glm::ivec2(x, y), tile);
          else if (distanceSquared <= featherRangeSquared)
            tilesInFeatherRange.emplace(position + glm::ivec2(x, y), FeatherTile(tile, (distanceSquared - raiseRangeSquared) / (raiseRangeSquared * featherRangeSquared)));
        }
      }
    }

    for (const auto& tilePair : tilesInRange)
    {
      Tile* tile = tilePair.second;
      const glm::ivec2 tilePosition = tilePair.first;

      for (int i = 0; i < VKREN_TILE_EDGE_COUNT; i++)
        tile->SetHeight(i, tile->GetHeight(i) + amount);

      Terrain::AdjustSurroundingTilesTo(tile, tilePosition);
    }

    for (const auto& tilePair : tilesInFeatherRange)
    {
      const FeatherTile tile = tilePair.second;
      const glm::ivec2 tilePosition = tilePair.first;

      for (int i = 0; i < VKREN_TILE_EDGE_COUNT; i++)
        tile.tile->SetHeight(i, tile.tile->GetHeight(i) + static_cast<uint32_t>(static_cast<float>(amount) * (1.0f - tile.distance)));

      Terrain::AdjustSurroundingTilesTo(tile.tile, tilePosition);
    }

    m_Saved = false;
  }

  const std::vector<Vertex>& Terrain::GetVertices()
  {
    if (!m_Saved)
      Terrain::CalculateVerticesAndIndices();
    return m_Vertices;
  }

  const std::vector<uint32_t>& Terrain::GetIndices()
  {
    if (!m_Saved)
      Terrain::CalculateVerticesAndIndices();
    return m_Indices;
  }

  void Terrain::CalculateVerticesAndIndices()
  {
    m_Vertices.clear();
    m_Indices.clear();
    m_Indices.shrink_to_fit();
    m_Indices.reserve(m_Tiles.size() * VKREN_TILE_VERTEX_COUNT);

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const auto& tilePair : m_Tiles)
    {
      const Tile& tile = tilePair.second;
      const glm::ivec2 tilePosition = tilePair.first;
      const glm::vec3 basePosition = glm::vec3(tilePosition.x * 10.0f, tilePosition.y * 10.0f, 0.0f);
      constexpr float halfSize = 5.0f;

      std::array<Vertex, VKREN_TILE_VERTEX_COUNT> vertices;
      vertices[0].Position      = glm::vec3(halfSize, -halfSize, static_cast<float>(tile.GetHeight(Edge::BottomRight)) * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition;
      vertices[0].TextureCoord  = glm::vec2(1.0f, 1.0f);
      vertices[1].Position      = glm::vec3(halfSize, halfSize, static_cast<float>(tile.GetHeight(Edge::TopRight)) * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition;
      vertices[1].TextureCoord  = glm::vec2(1.0f, 0.0f);
      vertices[2].Position      = glm::vec3(-halfSize, -halfSize, static_cast<float>(tile.GetHeight(Edge::BottomLeft)) * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition;
      vertices[2].TextureCoord  = glm::vec2(0.0f, 1.0f);

      vertices[3].Position      = vertices[1].Position;
      vertices[3].TextureCoord  = vertices[1].TextureCoord;
      vertices[4].Position      = glm::vec3(-halfSize, halfSize, static_cast<float>(tile.GetHeight(Edge::TopLeft)) * VKREN_TILE_FEET_TO_METERS_HEIGHT) + basePosition;
      vertices[4].TextureCoord  = glm::vec2(0.0f, 0.0f);
      vertices[5].Position      = vertices[2].Position;
      vertices[5].TextureCoord  = vertices[2].TextureCoord;

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

    m_Saved = true;
  }

  Tile* Terrain::FindTile(glm::ivec2 position)
  {
    auto it = m_Tiles.find(position);
    if (it != m_Tiles.end())
      return &it->second;
    return nullptr;
  }

  void Terrain::AdjustSurroundingTilesTo(Tile* tile, glm::ivec2 position)
  {
    Tile* bottomTile = Terrain::FindTile(position + glm::ivec2(0, -1));
    if (bottomTile)
    {
      bottomTile->SetHeight(Edge::TopLeft, tile->GetHeight(Edge::BottomLeft));
      bottomTile->SetHeight(Edge::TopRight, tile->GetHeight(Edge::BottomRight));
    }

    Tile* bottomLeftTile = Terrain::FindTile(position + glm::ivec2(-1, -1));
    if (bottomLeftTile)
      bottomLeftTile->SetHeight(Edge::TopRight, tile->GetHeight(Edge::BottomLeft));

    Tile* leftTile = Terrain::FindTile(position + glm::ivec2(-1, 0));
    if (leftTile)
    {
      leftTile->SetHeight(Edge::TopRight, tile->GetHeight(Edge::TopLeft));
      leftTile->SetHeight(Edge::BottomRight, tile->GetHeight(Edge::BottomLeft));
    }

    Tile* leftTopTile = Terrain::FindTile(position + glm::ivec2(-1, 1));
    if (leftTopTile)
      leftTopTile->SetHeight(Edge::BottomRight, tile->GetHeight(Edge::TopLeft));

    Tile* topTile = Terrain::FindTile(position + glm::ivec2(0, 1));
    if (topTile)
    {
      topTile->SetHeight(Edge::BottomRight, tile->GetHeight(Edge::TopRight));
      topTile->SetHeight(Edge::BottomLeft, tile->GetHeight(Edge::TopLeft));
    }

    Tile* topRightTile = Terrain::FindTile(position + glm::ivec2(1, 1));
    if (topRightTile)
      topRightTile->SetHeight(Edge::BottomLeft, tile->GetHeight(Edge::TopRight));

    Tile* rightTile = Terrain::FindTile(position + glm::ivec2(1, 0));
    if (rightTile)
    {
      rightTile->SetHeight(Edge::TopLeft, tile->GetHeight(Edge::TopRight));
      rightTile->SetHeight(Edge::BottomLeft, tile->GetHeight(Edge::BottomRight));
    }

    Tile* rightBottomTile = Terrain::FindTile(position + glm::ivec2(1, -1));
    if (rightBottomTile)
      rightBottomTile->SetHeight(Edge::TopLeft, tile->GetHeight(Edge::BottomRight));
  }

}