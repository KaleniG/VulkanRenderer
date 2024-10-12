#pragma once

#include"VulkanRenderer/Core/Base.h"

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Buffer.h"

namespace vkren
{

  class IndexBufferM : public Buffer
  {
  public:
    uint32_t GetIndexCount() const { return m_IndexCount; }

    static Ref<IndexBufferM> Create(const std::vector<uint32_t>& indices);
    static Ref<IndexBufferM> Create(VkDeviceSize size, bool copyable = false);

  private:
    uint32_t m_IndexCount;
  };

}