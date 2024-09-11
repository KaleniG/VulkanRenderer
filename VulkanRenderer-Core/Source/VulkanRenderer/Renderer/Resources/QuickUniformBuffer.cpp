#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/QuickUniformBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  QuickUniformBuffer::~QuickUniformBuffer()
  {
    QuickUniformBuffer::Unmap();
  }

  void QuickUniformBuffer::Map()
  {
    if (m_Mapped)
    {
      CORE_WARN("[SYSTEM] Mapping an already mapped memory for the buffer '{}'", (int)m_Buffer);
      return;
    }

    VkResult result = vkMapMemory(Renderer::GetDevice().GetLogical(), m_Memory, 0, m_Size, 0, &m_Data);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to map the memory. {}", Utils::VkResultToString(result));

    m_Mapped = true;
  }

  void QuickUniformBuffer::Update(void* data)
  {
    CORE_ASSERT(m_Mapped, "[VULKAN/SYSTEM] This buffer's memory isn't mapped");

    std::memcpy(m_Data, data, static_cast<size_t>(m_Size));

    VkMappedMemoryRange memoryRange = {};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.memory = m_Memory;
    memoryRange.offset = 0;
    memoryRange.size = m_Size;

    VkResult result = vkInvalidateMappedMemoryRanges(Renderer::GetDevice().GetLogical(), 1, &memoryRange);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to invalidate mapped memory. {}", Utils::VkResultToString(result));
  }

  void QuickUniformBuffer::Unmap()
  {
    if (!m_Mapped)
    {
      CORE_WARN("[SYSTEM] Unmapping already unmapped memory for the buffer '{}'", (int)m_Buffer);
      return;
    }

    vkUnmapMemory(Renderer::GetDevice().GetLogical(), m_Memory);
  }

  Ref<QuickUniformBuffer> QuickUniformBuffer::Create(const UniformBufferCreateInfo& info)
  {
    return QuickUniformBuffer::Create(info.Size, info.Copyable);
  }

  Ref<QuickUniformBuffer> QuickUniformBuffer::Create(const VkDeviceSize& size, bool copyable)
  {
    Ref<QuickUniformBuffer> buffer = CreateRef<QuickUniformBuffer>();

    buffer->CreateUniformBuffer(size, (copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    buffer->Map();

    return buffer;
  }

}