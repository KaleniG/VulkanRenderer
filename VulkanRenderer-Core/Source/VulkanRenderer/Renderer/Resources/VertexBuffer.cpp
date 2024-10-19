#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/VertexBuffer.h"
#include "VulkanRenderer/Renderer/Resources/StagingBuffer.h"
#include "VulkanRenderer/Renderer/Vertex.h"


namespace vkren
{
  
  Ref<VertexBuffer> VertexBuffer::Create(const std::vector<Vertex>& vertices)
  {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    StagingBuffer stagingBuffer = StagingBuffer::Create(bufferSize);
    stagingBuffer.Map();
    stagingBuffer.Update((void*)vertices.data());
    stagingBuffer.Unmap();

    Ref<VertexBuffer> buffer = VertexBuffer::Create(bufferSize);
    stagingBuffer.CopyToBuffer(*buffer.get());

    return buffer;
  }

  Ref<VertexBuffer> VertexBuffer::Create(VkDeviceSize size, bool copyable)
  {
    Ref<VertexBuffer> buffer = CreateRef<VertexBuffer>();
    buffer->CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
    return buffer;
  }

}