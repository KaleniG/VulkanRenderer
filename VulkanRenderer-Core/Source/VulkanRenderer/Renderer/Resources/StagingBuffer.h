#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/MappableBuffer.h"

namespace vkren
{

  class StagingBuffer : public MappableBuffer
  {
  public:
    static StagingBuffer Create(Image& src_image, bool copy = true);
    static StagingBuffer Create(Buffer& src_buffer, bool copy = true);
    static StagingBuffer Create(const BufferCreateInfo& info);
    static StagingBuffer Create(const VkDeviceSize& size);
  };

}