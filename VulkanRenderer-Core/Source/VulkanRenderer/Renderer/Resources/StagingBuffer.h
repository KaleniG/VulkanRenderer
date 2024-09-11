#pragma once

#include "VulkanRenderer/Renderer/Resources/Buffer.h"

namespace vkren
{

  enum class FlushType
  {
    Invalidate, Flush
  };

  struct StagingBufferCreateInfo
  {
    VkDeviceSize Size;
    bool Mappable = true;
    bool Copyable = false;
  };

  class StagingBuffer : public Buffer
  {
  public:
    void Map(const VkDeviceSize& size = VK_WHOLE_SIZE, const VkDeviceSize& offset = 0);
    void Update(void* data, const VkDeviceSize& size, bool flush = true);
    void* GetData(bool flush = true);
    void Flush(const FlushType& flush_type, const VkDeviceSize& size = 0);
    void Unmap();

    static StagingBuffer Create(Image& src_image, bool copy = true, bool mappable = true, bool copyable = false);
    static StagingBuffer Create(Buffer& src_buffer, bool copy = true,  bool mappable = true, bool copyable = false);
    static StagingBuffer Create(const StagingBufferCreateInfo& info);
    static StagingBuffer Create(const VkDeviceSize& size, bool mappable = true, bool copyable = false);

  private:
    void* m_Data;
    bool m_Mapped;
    VkDeviceSize m_LastOffset;
    VkDeviceSize m_LastSize;
  };

}