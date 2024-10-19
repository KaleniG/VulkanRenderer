#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Buffer.h"

namespace vkren
{

  class IndexBuffer : public Buffer
  {
  public:
    uint32_t GetIndexCount() const { return m_IndexCount; }

    static Ref<IndexBuffer> Create(const std::vector<uint32_t>& indices);
    static Ref<IndexBuffer> Create(VkDeviceSize size, bool copyable = false);

  private:
    uint32_t m_IndexCount;
  };

}