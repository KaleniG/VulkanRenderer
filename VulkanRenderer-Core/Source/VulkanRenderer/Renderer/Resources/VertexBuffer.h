#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Buffer.h"

namespace vkren
{

  struct Vertex;
  class VertexBuffer : public Buffer
  {
  public:
    static Ref<VertexBuffer> Create(const std::vector<Vertex>& vertices);
    static Ref<VertexBuffer> Create(VkDeviceSize size, bool copyable = false);
  };


}