#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/MappableBuffer.h"

namespace vkren
{

  class MStorageBuffer : public MappableBuffer
  {
  public:
    void* GetMappedData(const VkDeviceSize& size = 0, const VkDeviceSize& offset = 0);

    static MStorageBuffer Create(const BufferCreateInfo& info);
    static MStorageBuffer Create(const VkDeviceSize& size, bool copyable = false);
  };

}
