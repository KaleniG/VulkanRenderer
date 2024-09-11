#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/UniformBuffer.h"

namespace vkren
{

  void UniformBuffer::CreateUniformBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& memory_properties)
  {
    Buffer::CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | usage, memory_properties, size);
  }

}