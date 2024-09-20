#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/MUniformBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  MUniformBuffer::~MUniformBuffer()
  {
    MUniformBuffer::Unmap();
  }

  Ref<MUniformBuffer> MUniformBuffer::Create(const BufferCreateInfo& info)
  {
    return MUniformBuffer::Create(info.Size, info.Copyable);
  }

  Ref<MUniformBuffer> MUniformBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    Ref<MUniformBuffer> buffer = CreateRef<MUniformBuffer>();

    buffer->CreateMappableBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size);
    buffer->Map();

    return buffer;
  }

}