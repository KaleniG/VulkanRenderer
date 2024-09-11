#pragma once

#include "VulkanRenderer/Renderer/Resources/Buffer.h"

namespace vkren
{

  struct UniformBufferCreateInfo
  {
    VkDeviceSize Size;
    bool Copyable = false;
  };

  class UniformBuffer : public Buffer
  {
  protected:
    void CreateUniformBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& memory_properties);

  protected:
    void* m_Data = nullptr;
  };

}