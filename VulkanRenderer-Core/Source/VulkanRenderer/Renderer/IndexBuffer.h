#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  class IndexBuffer
  {
  public:
    IndexBuffer(const std::vector<uint32_t>& indices);
    ~IndexBuffer();

    const VkBuffer& Get() const { return m_Buffer; }
    const uint32_t& GetSize() const { return m_Size; }

  private:
    Ref<Device> r_Device;

    uint32_t m_Size;
    VkBuffer m_Buffer;
    VkDeviceMemory m_Memory;
  };

}