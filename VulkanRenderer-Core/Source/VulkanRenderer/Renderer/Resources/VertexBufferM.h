#pragma once

#include "VulkanRenderer/Core/Base.h"

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Buffer.h"

namespace vkren
{

  struct Vertex;
  class VertexBufferM : public Buffer
  {
  public:
    static Ref<VertexBufferM> Create(const std::vector<Vertex>& vertices);
    static Ref<VertexBufferM> Create(VkDeviceSize size, bool copyable = false);
  };


}