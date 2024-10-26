#include <vkrenpch.h>

#include "VulkanRenderer/GameComponents/Map/Tile.h"

namespace vkren
{
  Tile::Tile(std::array<uint32_t, VKREN_TILE_EDGE_COUNT> heights, const Ref<Texture>& texture, bool passable)
    : m_Heights(heights), r_Texture(texture), m_Passable(passable) {}

  Tile::Tile(uint32_t height, const Ref<Texture>& texture, bool passable)
    : m_Heights({height, height, height, height}), r_Texture(texture), m_Passable(passable) {}

  void Tile::SetHeight(uint32_t vertex, uint32_t value)
  {
    CORE_ASSERT(vertex < VKREN_TILE_EDGE_COUNT, "[SYSTEM] A tile has only four vertices");
    m_Heights[vertex] = value;
  }

  void Tile::SetHeight(Edge edge, uint32_t value)
  {
    switch (edge)
    {
      case Edge::TopLeft:
        m_Heights[0] = value;
        break;
      case Edge::TopRight:
        m_Heights[1] = value;
        break;
      case Edge::BottomLeft:
        m_Heights[2] = value;
        break;
      case Edge::BottomRight:
        m_Heights[3] = value;
        break;
      default:
        break;
    }
  }

  void Tile::SetHeights(std::array<uint32_t, VKREN_TILE_EDGE_COUNT> heights)
  {
    m_Heights = heights;
  }

  void Tile::SetHeights(uint32_t height)
  {
    m_Heights = { height, height, height, height };
  }

  void Tile::SetTexture(const Ref<Texture>& texture)
  {
    r_Texture = texture;
  }

  void Tile::SetPassable(bool passable)
  {
    m_Passable = passable;
  }

  uint32_t Tile::GetHeight(uint32_t vertex) const
  {
    CORE_ASSERT(vertex < VKREN_TILE_EDGE_COUNT, "[SYSTEM] A tile has only four vertices");
    return m_Heights[vertex];
  }

  uint32_t Tile::GetHeight(Edge edge) const
  {
    switch (edge)
    {
    case Edge::TopLeft:
      return m_Heights[0];
    case Edge::TopRight:
      return m_Heights[1];
    case Edge::BottomLeft:
      return m_Heights[2];
    case Edge::BottomRight:
      return m_Heights[3];
    default:
      return 0;
    }
  }

  const std::array<uint32_t, VKREN_TILE_EDGE_COUNT>& Tile::GetHeights() const
  {
    return m_Heights;
  }

  const Ref<Texture>& Tile::GetTexture() const
  {
    return r_Texture;
  }

  bool Tile::IsPassable() const
  {
    return m_Passable;
  }

}