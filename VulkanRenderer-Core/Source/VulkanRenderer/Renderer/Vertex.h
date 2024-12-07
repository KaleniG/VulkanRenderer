#pragma once

#include <vulkan/vulkan.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace vkren
{

  struct alignas(16) ModelViewProjectionUBO
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  struct alignas(16) Vertex
  {
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 TextureCoord;

    bool operator==(const Vertex& other) const { return Position == other.Position && Color == other.Color && TextureCoord == other.TextureCoord; }
  };

  struct alignas(16) TerrainVertex
  {
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 TextureCoord;

    bool operator==(const TerrainVertex& other) const { return Position == other.Position && Color == other.Color && TextureCoord == other.TextureCoord; }
  };

}

namespace std
{
  template<> struct hash<vkren::Vertex>
  {
    size_t operator()(vkren::Vertex const& vertex) const
    {
      size_t seed = 0;
      hash_combine(seed, vertex.Position);
      hash_combine(seed, vertex.Color);
      hash_combine(seed, vertex.TextureCoord);
      return seed;
    }

  private:
    template<typename T>
    void hash_combine(size_t& seed, T const& v) const
    {
      seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  };

  template<> struct hash<vkren::TerrainVertex>
  {
    size_t operator()(vkren::TerrainVertex const& vertex) const
    {
      size_t seed = 0;
      hash_combine(seed, vertex.Position);
      hash_combine(seed, vertex.Color);
      hash_combine(seed, vertex.TextureCoord);
      return seed;
    }

  private:
    template<typename T>
    void hash_combine(size_t& seed, T const& v) const
    {
      seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  };
}