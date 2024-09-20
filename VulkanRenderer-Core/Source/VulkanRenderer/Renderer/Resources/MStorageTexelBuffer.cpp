#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/MStorageTexelBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  MStorageTexelBuffer::~MStorageTexelBuffer()
  {
    vkDestroyBufferView(Renderer::GetDevice().GetLogical(), m_View, VK_NULL_HANDLE);
  }

  void* MStorageTexelBuffer::GetMappedData(const VkDeviceSize& size, const VkDeviceSize& offset)
  {
    CORE_ASSERT(m_Mapped, "[VULKAN/SYSTEM] This buffer's memory isn't mapped");

    VkMappedMemoryRange memoryRange = {};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.memory = m_Memory;
    memoryRange.offset = offset;
    memoryRange.size = size ? size : VK_WHOLE_SIZE;

    VkResult result = vkFlushMappedMemoryRanges(Renderer::GetDevice().GetLogical(), 1, &memoryRange);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to flush mapped memory. {}", Utils::VkResultToString(result));

    return m_Data;
  }

  MStorageTexelBuffer MStorageTexelBuffer::Create(const MStorageTexelBufferCreateInfo& info)
  {
    return MStorageTexelBuffer::Create(info.Buffer.Size, info.Buffer.Copyable, info.View.Format, info.View.Range, info.View.Offset);
  }

  MStorageTexelBuffer MStorageTexelBuffer::Create(const VkDeviceSize& size, bool copiable, const VkFormat& format, const VkDeviceSize& range, const VkDeviceSize& offset)
  {
    MStorageTexelBuffer buffer;
    buffer.CreateMappableBuffer((copiable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT : 0) | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT, size);

    CORE_ASSERT(Renderer::GetDevice().GetFormatProperties(format).bufferFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT, "[VULKAN/SYSTEM] The specified format cannot be used for storage texel buffers");
    CORE_ASSERT(offset % Renderer::GetDevice().GetPhysicalDeviceProperties().limits.minTexelBufferOffsetAlignment == 0, "[VULKAN/SYSTEM] The offset for a buffer view must be specified as a multiple of 'texelBlockAlignment' of this device (texelBlockAlignment:{})", Renderer::GetDevice().GetPhysicalDeviceProperties().limits.minTexelBufferOffsetAlignment);
    CORE_ASSERT(offset + range <= buffer.m_Size, "[VULKAN/SYSTEM] The buffer view's data range should be inbounds of the buffer's size");

    VkBufferViewCreateInfo bufferViewCreateInfo = {};
    bufferViewCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    bufferViewCreateInfo.buffer = buffer.m_Buffer;
    bufferViewCreateInfo.format = format;
    bufferViewCreateInfo.offset = offset;
    bufferViewCreateInfo.range = (range) ? range : VK_WHOLE_SIZE;

    VkResult result = vkCreateBufferView(Renderer::GetDevice().GetLogical(), &bufferViewCreateInfo, VK_NULL_HANDLE, &buffer.m_View);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create this buffer's view. {}", Utils::VkResultToString(result));

    return buffer;
  }

}