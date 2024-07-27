#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/UniformBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  UniformBuffer::UniformBuffer(size_t buffer_size)
    : r_Device(Renderer::GetDeviceRef()), m_Size(buffer_size)
  {
    Device& device = *r_Device.get();

    device.CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_Memory);
    vkMapMemory(device.GetLogical(), m_Memory, 0, buffer_size, 0, &m_MappedBuffer);
  }

  UniformBuffer::~UniformBuffer()
  {
    Device& device = *r_Device.get();

    vkDestroyBuffer(device.GetLogical(), m_Buffer, VK_NULL_HANDLE);
    vkFreeMemory(device.GetLogical(), m_Memory, VK_NULL_HANDLE);
  }

}