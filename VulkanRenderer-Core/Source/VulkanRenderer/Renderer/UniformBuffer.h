#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  class UniformBuffer
  {
  public:
    UniformBuffer(size_t buffer_size);
    ~UniformBuffer();

    const VkBuffer& GetBuffer() const { return m_Buffer; }
    const size_t& GetSize() const { return m_Size; }

  private:
    Ref<Device> r_Device;

    size_t m_Size;
    VkBuffer m_Buffer;
    VkDeviceMemory m_Memory;
    void* m_MappedBuffer;
  };

}