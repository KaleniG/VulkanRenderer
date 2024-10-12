#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/VertexBufferM.h"
#include "VulkanRenderer/Renderer/Resources/StagingBuffer.h"
#include "VulkanRenderer/Renderer/Vertex.h"


namespace vkren
{
  
  Ref<VertexBufferM> VertexBufferM::Create(const std::vector<Vertex>& vertices)
  {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    StagingBuffer stagingBuffer = StagingBuffer::Create(bufferSize);
    stagingBuffer.Map();
    stagingBuffer.Update((void*)vertices.data());
    stagingBuffer.Unmap();

    Ref<VertexBufferM> buffer = VertexBufferM::Create(bufferSize);
    stagingBuffer.CopyToBuffer(*buffer.get());

    return buffer;
  }

  Ref<VertexBufferM> VertexBufferM::Create(VkDeviceSize size, bool copyable)
  {
    Ref<VertexBufferM> buffer = CreateRef<VertexBufferM>();
    buffer->CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
    return buffer;
  }

}