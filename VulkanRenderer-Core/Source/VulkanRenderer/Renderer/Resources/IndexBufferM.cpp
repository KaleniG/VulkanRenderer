#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/StagingBuffer.h"
#include "VulkanRenderer/Renderer/Resources/IndexBufferM.h"

namespace vkren
{

  Ref<IndexBufferM> IndexBufferM::Create(const std::vector<uint32_t>& indices)
  {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    StagingBuffer stagingBuffer = StagingBuffer::Create(bufferSize);
    stagingBuffer.Map();
    stagingBuffer.Update((void*)indices.data());
    stagingBuffer.Unmap();

    Ref<IndexBufferM> buffer = IndexBufferM::Create(bufferSize);
    stagingBuffer.CopyToBuffer(*buffer.get());

    buffer->m_IndexCount = indices.size();

    return buffer;
  }

  Ref<IndexBufferM> IndexBufferM::Create(VkDeviceSize size, bool copyable)
  {
    Ref<IndexBufferM> buffer = CreateRef<IndexBufferM>();
    buffer->CreateBuffer((copyable ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size);
    return buffer;
  }

}