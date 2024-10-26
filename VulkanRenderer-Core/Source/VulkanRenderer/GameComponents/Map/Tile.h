#pragma once

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/Resources/Texture.h"

#define VKREN_TILE_EDGE_COUNT 4

namespace vkren
{

  enum class Edge
  {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
  };

  class Tile
  {
  public:
    Tile() = default;
    Tile(std::array<uint32_t, VKREN_TILE_EDGE_COUNT> heights, const Ref<Texture>& texture, bool passable);
    Tile(uint32_t height, const Ref<Texture>& texture, bool passable);

    void SetHeight(uint32_t vertex, uint32_t value);
    void SetHeight(Edge edge, uint32_t value);
    void SetHeights(std::array<uint32_t, VKREN_TILE_EDGE_COUNT> heights);
    void SetHeights(uint32_t height);
    void SetTexture(const Ref<Texture>& texture);
    void SetPassable(bool passable);

    uint32_t GetHeight(uint32_t vertex) const;
    uint32_t GetHeight(Edge edge) const;
    const std::array<uint32_t, VKREN_TILE_EDGE_COUNT>& GetHeights() const;
    const Ref<Texture>& GetTexture() const;
    bool IsPassable() const;

  private:
    std::array<uint32_t, VKREN_TILE_EDGE_COUNT> m_Heights;
    Ref<Texture> r_Texture;
    bool m_Passable;
  };

}