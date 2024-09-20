#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/UniformBuffer.h"

namespace vkren
{

  Ref<UniformBuffer> UniformBuffer::Create(const BufferCreateInfo& info)
  {
    return UniformBuffer::Create(info.Size, info.Copyable);
  }

  Ref<UniformBuffer> UniformBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    Ref<UniformBuffer> buffer = CreateRef<UniformBuffer>();

    buffer->CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);

    return buffer;
  }

}