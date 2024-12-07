#pragma once

#include <glm/glm.hpp>

namespace vkren
{

  enum class Direction : uint8_t
  {
    None    = 0,
    Top     = 1 << 0,
    Right   = 1 << 1,
    Bottom  = 1 << 2,
    Left    = 1 << 3,

    TopLeft     = Top | Left,
    TopRight    = Top | Right,
    BottomLeft  = Bottom | Left,
    BottomRight = Bottom | Right,

    North = Top,
    East  = Right,
    South = Bottom,
    West  = Left,

    NorthEast = TopLeft,
    NorthWest = TopRight,
    SouthEast = BottomLeft,
    SouthWest = BottomRight,

    AllDirections = Top | Right | Bottom | Left
  };

  typedef uint8_t DirectionMask;

  inline DirectionMask operator|(DirectionMask lhs, Direction rhs)
  {
    return lhs | static_cast<std::underlying_type_t<Direction>>(rhs);
  }

  inline DirectionMask operator&(DirectionMask lhs, Direction rhs)
  {
    return lhs & static_cast<std::underlying_type_t<Direction>>(rhs);
  }

  inline DirectionMask operator^(DirectionMask lhs, Direction rhs)
  {
    return lhs ^ static_cast<std::underlying_type_t<Direction>>(rhs);
  }

  inline DirectionMask& operator^=(DirectionMask& lhs, Direction rhs) 
  {
    lhs = lhs ^ static_cast<std::underlying_type_t<Direction>>(rhs);
    return lhs;
  }

  inline DirectionMask& operator|=(DirectionMask& lhs, Direction rhs)
  {
    lhs = lhs | static_cast<std::underlying_type_t<Direction>>(rhs);
    return lhs;
  }

  inline DirectionMask& operator&=(DirectionMask& lhs, Direction rhs)
  {
    lhs = lhs & static_cast<std::underlying_type_t<Direction>>(rhs);
    return lhs;
  }

  class Heightmap
  {
  public:
    Heightmap() = default;
    Heightmap(glm::uvec2 size, float start_height);
    Heightmap(const std::filesystem::path& image, float max_height);
    Heightmap(const Heightmap& other);
    ~Heightmap();

    void SetHeight(uint32_t x, uint32_t y, float value);
    void Resize(glm::uvec2 size);
    void Expand(DirectionMask direction, glm::uvec2::value_type amount);

    float GetHeight(uint32_t x, uint32_t y) const;
    glm::uvec2 GetSize() const { return m_Size; }

    Heightmap& operator=(const Heightmap& other);

  private:
    void AllocateHeightmap(glm::uvec2 size, float initialValue = 0.0f);
    void DeallocateHeightmap();

  private:
    glm::uvec2 m_Size;
    float m_StartHeight;
    float** m_Heightmap;
  };

}