#pragma once

#pragma once

#include <VulkanRenderer/Core/Base.h>

#include "VulkanRenderer/Renderer/Resources/UniformBuffer.h"

namespace vkren
{

  class StableUniformBuffer : public UniformBuffer
  {
  public:
    static Ref<StableUniformBuffer> Create(const UniformBufferCreateInfo& info);
    static Ref<StableUniformBuffer> Create(const VkDeviceSize& size, bool copyable = false);
  };

}