#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/VertexBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices)
    : r_Device(Renderer::GetDeviceRef())
  {
    Device& device = *r_Device.get();

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device.GetLogical(), stagingBufferMemory, 0, bufferSize, 0, &data);
    std::memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device.GetLogical(), stagingBufferMemory);

    device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_Memory);
    device.CmdCopyBufferToBuffer(stagingBuffer, m_Buffer, bufferSize);

    vkDestroyBuffer(device.GetLogical(), stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(device.GetLogical(), stagingBufferMemory, VK_NULL_HANDLE);
  }

  VertexBuffer::~VertexBuffer()
  {
    Device& device = *r_Device.get();

    vkDestroyBuffer(device.GetLogical(), m_Buffer, VK_NULL_HANDLE);
    vkFreeMemory(device.GetLogical(), m_Memory, VK_NULL_HANDLE);
  }

}