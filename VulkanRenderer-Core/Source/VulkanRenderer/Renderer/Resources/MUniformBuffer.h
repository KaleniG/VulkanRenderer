#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/MappableBuffer.h"

namespace vkren
{

  class MUniformBuffer : public MappableBuffer
  {
  public:
    ~MUniformBuffer();

    static Ref<MUniformBuffer> Create(const BufferCreateInfo& info);
    static Ref<MUniformBuffer> Create(const VkDeviceSize& size, bool copyable = false);
  };

}