#include <vkrenpch.h>

#include <stb/stb_image.h>

#include "VulkanRenderer/GameComponents/Map/Heightmap.h"

namespace vkren
{

  Heightmap::Heightmap(glm::uvec2 size, float start_height)
    : m_Size(size), m_StartHeight(start_height), m_Heightmap(nullptr)
  {
    Heightmap::AllocateHeightmap(size, start_height);
  }

  Heightmap::Heightmap(const std::filesystem::path& image, float max_height)
    : m_Size(0, 0), m_StartHeight(0.0f), m_Heightmap(nullptr)
  {
    CORE_ASSERT(image.extension().string() == ".png", "[SYSTEM] Only '.png' image format supported for now");

    int32_t width, height, channelsCount;
    stbi_uc* pixels = stbi_load(image.string().c_str(), &width, &height, &channelsCount, STBI_grey);
    CORE_ASSERT(pixels, "[STB] Failed to load the image");

    m_Size = glm::uvec2(width, height);
    Heightmap::AllocateHeightmap(m_Size);

    for (int32_t y = 0; y < height; ++y)
    {
      for (int32_t x = 0; x < width; ++x)
      {
        float normalizedHeight = pixels[y * width + x] / 255.0f;
        m_Heightmap[y][x] = normalizedHeight * max_height;
      }
    }
    
    stbi_image_free(pixels);
  }

  Heightmap::Heightmap(const Heightmap& other)
    : m_Size(other.m_Size), m_StartHeight(other.m_StartHeight), m_Heightmap(nullptr)
  {
    AllocateHeightmap(m_Size);

    for (uint32_t y = 0; y < m_Size.y; ++y)
      for (uint32_t x = 0; x < m_Size.x; ++x)
        m_Heightmap[y][x] = other.m_Heightmap[y][x];
  }

  Heightmap::~Heightmap()
  {
    Heightmap::DeallocateHeightmap();
  }

  float Heightmap::GetHeight(uint32_t x, uint32_t y) const
  {
    CORE_ASSERT(x < m_Size.x && y < m_Size.y, "[SYSTEM] Out of bounds");
    return m_Heightmap[y][x];
  }

  Heightmap& Heightmap::operator=(const Heightmap& other)
  {
    if (this == &other)
      return *this;

    Heightmap::DeallocateHeightmap();

    m_Size = other.m_Size;
    m_StartHeight = other.m_StartHeight;
    Heightmap::AllocateHeightmap(m_Size);

    for (uint32_t y = 0; y < m_Size.y; ++y)
      for (uint32_t x = 0; x < m_Size.x; ++x)
        m_Heightmap[y][x] = other.m_Heightmap[y][x];

    return *this;
  }

  void Heightmap::AllocateHeightmap(glm::uvec2 size, float initialValue)
  {
    m_Heightmap = new float*[size.y];
    for (uint32_t y = 0; y < size.y; ++y)
    {
      m_Heightmap[y] = new float[size.x];
      std::fill(m_Heightmap[y], m_Heightmap[y] + size.x, initialValue);
    }
  }

  void Heightmap::DeallocateHeightmap()
  {
    if (m_Heightmap)
    {
      for (uint32_t y = 0; y < m_Size.y; ++y)
          delete[] m_Heightmap[y];
      delete[] m_Heightmap;
      m_Heightmap = nullptr;
    }
  }

  void Heightmap::SetHeight(uint32_t x, uint32_t y, float value)
  {
    CORE_ASSERT(x < m_Size.x && y < m_Size.y, "[SYSTEM] Out of bounds");
    m_Heightmap[y][x] = value;
  }

  void Heightmap::Resize(glm::uvec2 size)
  {
    float** newHeightmap = new float* [size.y];
    for (uint32_t y = 0; y < size.y; ++y)
    {
      newHeightmap[y] = new float[size.x];
      std::fill(newHeightmap[y], newHeightmap[y] + size.x, m_StartHeight);
    }

    glm::uvec2 copyRange;
    copyRange.y = std::min(m_Size.y, size.y);
    copyRange.x = std::min(m_Size.x, size.x);

    for (uint32_t y = 0; y < copyRange.y; ++y)
      for (uint32_t x = 0; x < copyRange.x; ++x)
        newHeightmap[y][x] = m_Heightmap[y][x];

    Heightmap::DeallocateHeightmap();
    m_Heightmap = newHeightmap;
    m_Size = size;
  }

  void Heightmap::Expand(DirectionMask direction, glm::uvec2::value_type amount)
  {
    glm::uvec2 newSize = m_Size;
    glm::uvec2 displacement(0, 0);

    if (direction & Direction::Bottom)
      newSize.y += amount;
    if (direction & Direction::Right)
      newSize.x += amount;
    if (direction & Direction::Top)
    {
      displacement.y += amount;
      newSize.y += amount;
    }
    if (direction & Direction::Left)
    {
      displacement.x += amount;
      newSize.x += amount;
    }

    float** newHeightmap = new float* [newSize.y];
    for (uint32_t y = 0; y < newSize.y; ++y)
    {
      newHeightmap[y] = new float[newSize.x];
      std::fill(newHeightmap[y], newHeightmap[y] + newSize.x, m_StartHeight);
    }

    for (uint32_t y = 0; y < m_Size.y; ++y)
      for (uint32_t x = 0; x < m_Size.x; ++x)
        newHeightmap[y + displacement.y][x + displacement.x] = m_Heightmap[y][x];

    Heightmap::DeallocateHeightmap();
    m_Heightmap = newHeightmap;
    m_Size = newSize;
  }

}