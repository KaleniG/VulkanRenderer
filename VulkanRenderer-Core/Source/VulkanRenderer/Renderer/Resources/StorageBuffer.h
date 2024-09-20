#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Buffer.h"

namespace vkren
{

  class StorageBuffer : public Buffer
  {
  public:
    static StorageBuffer Create(const BufferCreateInfo& info);
    static StorageBuffer Create(const VkDeviceSize& size, bool copyable = false);
  };

}
