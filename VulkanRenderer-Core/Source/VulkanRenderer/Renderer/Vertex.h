#pragma once

#include <vulkan/vulkan.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace vkren
{

  struct Vertex
  {
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 TextureCoord;

    static VkVertexInputBindingDescription GetBindingDescription() 
    {
      VkVertexInputBindingDescription bindingDescription{};
      bindingDescription.binding = 0;
      bindingDescription.stride = sizeof(Vertex);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() 
    {
      std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions;

      attributeDescriptions[0].binding = 0;
      attributeDescriptions[0].location = 0;
      attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[0].offset = offsetof(Vertex, Position);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[1].offset = offsetof(Vertex, Color);

      attributeDescriptions[2].binding = 0;
      attributeDescriptions[2].location = 2;
      attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
      attributeDescriptions[2].offset = offsetof(Vertex, TextureCoord);

      return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const { return Position == other.Position && Color == other.Color && TextureCoord == other.TextureCoord; }
  };

}

namespace std 
{
  template<> struct hash<vkren::Vertex> 
  {
    size_t operator()(vkren::Vertex const& vertex) const 
    {
      return ((hash<glm::vec3>()(vertex.Position) ^ (hash<glm::vec3>()(vertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.TextureCoord) << 1);
    }
  };
}