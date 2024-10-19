#pragma once

#include <cstdint>

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/Resources/Texture.h"

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
    Tile(glm::ivec2 position, std::array<uint32_t, 4> heights, const Ref<Texture>& texture, bool passable);
    Tile(glm::ivec2 position, uint32_t height, const Ref<Texture>& texture, bool passable);

    void SetHeight(uint32_t vertex, uint32_t value);
    void SetHeight(Edge edge, uint32_t value);
    void SetHeights(std::array<uint32_t, 4> heights);
    void SetTexture(const Ref<Texture> texture);
    void SetPassable(bool passable);

    uint32_t GetHeight(uint32_t vertex);
    uint32_t GetHeight(Edge edge);
    const std::array<uint32_t, 4>& GetHeights();
    const Ref<Texture>& GetTexture();
    bool IsPassable();

  private:
    glm::ivec2 m_Position;
    std::array<uint32_t, 4> m_Heights;
    Ref<Texture> r_Texture;
    bool m_Passable;
  };

}