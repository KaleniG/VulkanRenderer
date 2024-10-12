#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Image.h"
#include "VulkanRenderer/Renderer/Resources/StagingBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  StagingBuffer StagingBuffer::Create(Image& src_image, bool copy)
  {
    StagingBuffer buffer = StagingBuffer::Create(src_image.GetSize());

    if (copy)
      src_image.CopyToBuffer(static_cast<Buffer&>(buffer));

    return buffer;
  }

  StagingBuffer StagingBuffer::Create(Buffer& src_buffer, bool copy)
  {
    StagingBuffer& src_buffer_temp = static_cast<StagingBuffer&>(src_buffer);
    StagingBuffer buffer = StagingBuffer::Create(src_buffer_temp.m_Size);

    if (copy)
      src_buffer.CopyToBuffer(static_cast<Buffer&>(buffer));

    return buffer;
  }

  StagingBuffer StagingBuffer::Create(const BufferCreateInfo& info)
  {
    return StagingBuffer::Create(info.Size);
  }

  StagingBuffer StagingBuffer::Create(const VkDeviceSize& size)
  {
    StagingBuffer buffer;
    buffer.CreateMappableBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size);
    return buffer;
  }

}