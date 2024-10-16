#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Vertex.h"
#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  class VertexBuffer
  {
  public:
    VertexBuffer(const std::vector<Vertex>& vertices);
    ~VertexBuffer();

    const VkBuffer& Get() const { return m_Buffer; }

  private:
    Ref<Device> r_Device;

    VkBuffer m_Buffer;
    VkDeviceMemory m_Memory;
  };

}