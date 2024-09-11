#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/StableUniformBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  Ref<StableUniformBuffer> StableUniformBuffer::Create(const UniformBufferCreateInfo& info)
  {
    return StableUniformBuffer::Create(info.Size, info.Copyable);
  }

  Ref<StableUniformBuffer> StableUniformBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    Ref<StableUniformBuffer> buffer = CreateRef<StableUniformBuffer>();

    buffer->CreateUniformBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, size);

    return buffer;
  }

}