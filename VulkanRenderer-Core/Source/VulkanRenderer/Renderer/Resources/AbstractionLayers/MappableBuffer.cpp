#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/MappableBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  void MappableBuffer::Map(const VkDeviceSize& size, const VkDeviceSize& offset)
  {
    CORE_ASSERT(!m_Mapped, "[VULKAN/SYSTEM] Memory already mapped");
    VkResult result = vkMapMemory(Renderer::GetDevice().GetLogical(), m_Memory, offset, size, 0, &m_Data);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to map the memory. {}", Utils::VkResultToString(result));

    m_Mapped = true;
  }

  void MappableBuffer::Update(void* data, const VkDeviceSize& size, const VkDeviceSize& offset, bool flush)
  {
    CORE_ASSERT(m_Mapped, "[VULKAN/SYSTEM] This buffer's memory isn't mapped");
    std::memcpy(m_Data, data, static_cast<size_t>(size ? size : m_Size));

    if (flush)
    {
      VkMappedMemoryRange memoryRange = {};
      memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      memoryRange.memory = m_Memory;
      memoryRange.offset = offset;
      memoryRange.size = size ? size : VK_WHOLE_SIZE;

      VkResult result = vkInvalidateMappedMemoryRanges(Renderer::GetDevice().GetLogical(), 1, &memoryRange);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to invalidate mapped memory. {}", Utils::VkResultToString(result));
    }
  }

  void MappableBuffer::Unmap()
  {
    CORE_ASSERT(m_Mapped, "[VULKAN/SYSTEM] Memory already unmapped");
    vkUnmapMemory(Renderer::GetDevice().GetLogical(), m_Memory);
    m_Mapped = false;
  }

  void MappableBuffer::CreateMappableBuffer(const VkBufferUsageFlags& usage, const VkDeviceSize& size)
  {
    Buffer::CreateBuffer(usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, size);
  }

}