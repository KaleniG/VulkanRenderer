#pragma once

#include <VulkanRenderer/Core/Base.h>

#include "VulkanRenderer/Renderer/Resources/UniformBuffer.h"

namespace vkren
{

  class QuickUniformBuffer : public UniformBuffer
  {
  public:
    ~QuickUniformBuffer();

    void Map();
    void Update(void* data);
    void Unmap();

    static Ref<QuickUniformBuffer> Create(const UniformBufferCreateInfo& info);
    static Ref<QuickUniformBuffer> Create(const VkDeviceSize& size, bool copyable = false);

  private:
    bool m_Mapped = false;
  };

}