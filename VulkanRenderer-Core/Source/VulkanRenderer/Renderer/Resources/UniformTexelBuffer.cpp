#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/UniformTexelBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  UniformTexelBuffer::~UniformTexelBuffer()
  {
    vkDestroyBufferView(Renderer::GetDevice().GetLogical(), m_View, VK_NULL_HANDLE);
  }

  UniformTexelBuffer UniformTexelBuffer::Create(const UniformTexelBufferCreateInfo& info)
  {
    return UniformTexelBuffer::Create(info.Buffer.Size, info.Buffer.Copyable, info.View.Format, info.View.Range, info.View.Offset);
  }

  UniformTexelBuffer UniformTexelBuffer::Create(const VkDeviceSize& size, bool copiable, const VkFormat& format, const VkDeviceSize& range, const VkDeviceSize& offset)
  {
    UniformTexelBuffer buffer;
    buffer.CreateBuffer((copiable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
    
    CORE_ASSERT(Renderer::GetDevice().GetFormatProperties(format).bufferFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT, "[VULKAN/SYSTEM] The specified format cannot be used for uniform texel buffers");
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