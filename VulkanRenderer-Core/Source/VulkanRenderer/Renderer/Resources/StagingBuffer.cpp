#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/StagingBuffer.h"
#include "VulkanRenderer/Renderer/Resources/Image.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  void StagingBuffer::Map(const VkDeviceSize& size, const VkDeviceSize& offset)
  {
    if (m_Mapped)
    {
      CORE_WARN("[SYSTEM] Mapping an already mapped memory for the buffer '{}'", (int)m_Buffer);
      return;
    }
    CORE_ASSERT(m_MemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "[VULKAN/SYSTEM] Cannot map memory that is not host-visible");

    VkResult result = vkMapMemory(Renderer::GetDevice().GetLogical(), m_Memory, offset, size, 0, &m_Data);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to map the memory. {}", Utils::VkResultToString(result));

    m_LastOffset = offset;
    m_LastSize = size;
    m_Mapped = true;
  }

  void StagingBuffer::Update(void* data, const VkDeviceSize& size, bool flush)
  {
    CORE_ASSERT(m_MemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "[VULKAN/SYSTEM] Cannot map memory that is not host-visible");
    CORE_ASSERT(m_Mapped, "[VULKAN/SYSTEM] This buffer's memory isn't mapped");
    std::memcpy(m_Data, data, static_cast<size_t>(size));

    if (flush)
      StagingBuffer::Flush(FlushType::Invalidate, size);
  }

  void* StagingBuffer::GetData(bool flush)
  {
    CORE_ASSERT(m_MemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "[VULKAN/SYSTEM] Cannot map memory that is not host-visible");
    CORE_ASSERT(m_Mapped, "[VULKAN/SYSTEM] This buffer's memory isn't mapped");

    if (flush)
      StagingBuffer::Flush(FlushType::Flush);

    return m_Data;
  }

  void StagingBuffer::Flush(const FlushType& flush_type, const VkDeviceSize& size)
  {
    if (!m_Mapped)
      CORE_WARN("[SYSTEM] Unmapping an already unmapped memory for the buffer '{}'", (int)m_Buffer);
    CORE_ASSERT(m_MemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "[VULKAN/SYSTEM] Cannot map memory that is not host-visible");

    VkMappedMemoryRange memoryRange = {};

    switch (flush_type)
    {
      case FlushType::Invalidate:
      {
        memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        memoryRange.memory = m_Memory;
        memoryRange.offset = m_LastOffset;
        memoryRange.size = size;

        VkResult result = vkInvalidateMappedMemoryRanges(Renderer::GetDevice().GetLogical(), 1, &memoryRange);
        CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to invalidate mapped memory. {}", Utils::VkResultToString(result));
      }
      case FlushType::Flush:
      {
        memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        memoryRange.memory = m_Memory;
        memoryRange.offset = m_LastOffset;
        memoryRange.size = m_LastSize;

        VkResult result = vkInvalidateMappedMemoryRanges(Renderer::GetDevice().GetLogical(), 1, &memoryRange);
        CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to flush mapped memory. {}", Utils::VkResultToString(result));
      }
    }
  }

  void StagingBuffer::Unmap()
  {
    if (!m_Mapped)
    {
      CORE_WARN("[SYSTEM] Unmapping already unmapped memory for the buffer '{}'", (int)m_Buffer);
      return;
    }
    CORE_ASSERT(m_MemoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "[VULKAN/SYSTEM] Cannot map memory that is not host-visible");

    vkUnmapMemory(Renderer::GetDevice().GetLogical(), m_Memory);

    m_Mapped = false;
  }

  StagingBuffer StagingBuffer::Create(Image& src_image, bool copy, bool mappable, bool copyable)
  {
    StagingBuffer buffer = StagingBuffer::Create(src_image.GetSize(), mappable, copyable);

    if (copy)
      src_image.CopyToBuffer(static_cast<Buffer&>(buffer));

    return buffer;
  }

  StagingBuffer StagingBuffer::Create(Buffer& src_buffer, bool copy, bool mappable, bool copyable)
  {
    StagingBuffer& src_buffer_temp = static_cast<StagingBuffer&>(src_buffer);
    StagingBuffer buffer = StagingBuffer::Create(src_buffer_temp.m_Size, mappable, copyable);

    if (copy)
      src_buffer.CopyToBuffer(static_cast<Buffer&>(buffer));

    return buffer;
  }

  StagingBuffer StagingBuffer::Create(const StagingBufferCreateInfo& info)
  {
    return StagingBuffer::Create(info.Size, info.Mappable, info.Copyable);
  }

  StagingBuffer StagingBuffer::Create(const VkDeviceSize& size, bool mappable, bool copyable)
  {
    StagingBuffer buffer;
    buffer.CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_TRANSFER_DST_BIT, (mappable ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), size);
    return buffer;
  }

}