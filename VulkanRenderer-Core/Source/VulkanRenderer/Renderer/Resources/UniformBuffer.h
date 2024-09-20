#pragma once

#include "VulkanRenderer/Core/Base.h"

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Buffer.h"

namespace vkren
{

  class UniformBuffer : public Buffer
  {
  public:
    static Ref<UniformBuffer> Create(const BufferCreateInfo& info);
    static Ref<UniformBuffer> Create(const VkDeviceSize& size, bool copyable = false);
  };

}