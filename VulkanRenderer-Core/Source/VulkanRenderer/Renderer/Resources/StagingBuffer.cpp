#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Image.h"
#include "VulkanRenderer/Renderer/Resources/StagingBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  StagingBuffer StagingBuffer::Create(Image& src_image, bool copy, bool copyable)
  {
    StagingBuffer buffer = StagingBuffer::Create(src_image.GetSize(), copyable);

    if (copy)
      src_image.CopyToBuffer(static_cast<Buffer&>(buffer));

    return buffer;
  }

  StagingBuffer StagingBuffer::Create(Buffer& src_buffer, bool copy, bool copyable)
  {
    StagingBuffer& src_buffer_temp = static_cast<StagingBuffer&>(src_buffer);
    StagingBuffer buffer = StagingBuffer::Create(src_buffer_temp.m_Size, copyable);

    if (copy)
      src_buffer.CopyToBuffer(static_cast<Buffer&>(buffer));

    return buffer;
  }

  StagingBuffer StagingBuffer::Create(const BufferCreateInfo& info)
  {
    return StagingBuffer::Create(info.Size, info.Copyable);
  }

  StagingBuffer StagingBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    StagingBuffer buffer;
    buffer.CreateMappableBuffer(copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0, size);
    return buffer;
  }

}