#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Buffer.h"
#include "VulkanRenderer/Renderer/Resources/StagingBuffer.h"

namespace vkren
{

  struct Vertex;
  struct TerrainVertex;
  class VertexBuffer : public Buffer
  {
  public:
    template<typename T>
    static Ref<VertexBuffer> Create(const std::vector<T>& vertices);
    static Ref<VertexBuffer> Create(VkDeviceSize size, bool copyable = false);
  };

}

#include "VulkanRenderer/Renderer/Resources/VertexBuffer.inl"