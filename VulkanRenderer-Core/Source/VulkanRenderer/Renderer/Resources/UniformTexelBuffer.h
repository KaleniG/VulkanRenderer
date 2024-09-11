#pragma once

#include "VulkanRenderer/Renderer/Resources/Buffer.h"

namespace vkren
{
  static struct BufferConfig
  {
    VkDeviceSize Size;
    bool Copyable = false;
  };

  static struct ViewConfig
  {
    VkFormat Format = VK_FORMAT_UNDEFINED;
    VkDeviceSize Range = 0;
    VkDeviceSize Offset = 0;
  };

  struct UniformTexelBufferCreateInfo
  {
    BufferConfig Buffer = {};
    ViewConfig View = {};
  };

  class UniformTexelBuffer : public Buffer
  {
  public:
    ~UniformTexelBuffer();

    const VkBufferView& GetView() const { return m_View; }

    static UniformTexelBuffer Create(const UniformTexelBufferCreateInfo& info);
    static UniformTexelBuffer Create
    (
      const VkDeviceSize& size, 
      bool copiable = false, 
      const VkFormat& format = VK_FORMAT_UNDEFINED, 
      const VkDeviceSize& range = 0, 
      const VkDeviceSize& offset = 0
    );

  private:
    VkBufferView m_View = VK_NULL_HANDLE;
  };

}