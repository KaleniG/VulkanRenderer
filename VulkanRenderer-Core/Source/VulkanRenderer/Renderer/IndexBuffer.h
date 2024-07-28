#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  class IndexBuffer
  {
  public:
    IndexBuffer(const std::vector<uint32_t>& indicies);
    ~IndexBuffer();

  private:
    Ref<Device> r_Device;

    VkBuffer m_Buffer;
    VkDeviceMemory m_Memory;
  };

}