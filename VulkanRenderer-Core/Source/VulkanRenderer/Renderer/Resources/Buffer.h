#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  struct BufferTransitionSpecifics
  {
    VkPipelineStageFlags PipelineStagesMask = VK_PIPELINE_STAGE_NONE;
    // RESERVED FOR FUTURE CHANGES
  };

  struct BufferToBufferCopySpecifics
  {
    std::vector<VkBufferCopy> CopyRegions = {};
    // RESERVED FOR FUTURE CHANGES
  };

  struct BufferToImageCopySpecifics
  {
    std::vector<VkBufferImageCopy> CopyData = {};
    bool GenerateMipmaps = false;
  };

  struct BufferCreateInfo
  {
    VkBufferUsageFlags Usage;
    VkMemoryPropertyFlags MemoryProperties;
    VkDeviceSize Size;
  };

  class Image; // Fuck includes
  class Buffer
  {
  public:
    ~Buffer();

    const VkBuffer& Get() const { return m_Buffer; }
    const VkDeviceSize& GetSize() const { return m_Size; }
    const VkAccessFlags& GetAccessMask() const { return m_CurrentAccessMask; } 
    const VkBufferUsageFlags& GetUsage() const { return m_Usage; }
    const VkDeviceMemory& GetMemory() const { return m_Memory; }

    void SetUsed(bool used) { m_Used = used; }
    void SetAccessMask(const VkAccessFlags& mask) { m_CurrentAccessMask = mask; }
    void SetPipelineStageMask(const VkPipelineStageFlags& mask) { m_CurrentPipelineStageMask = mask; }

    bool IsUsed() { return m_Used; }

    void Transition(const VkAccessFlags& new_access, const BufferTransitionSpecifics& specifics = {});
    void CopyToBuffer(Buffer& dst_buffer, const BufferToBufferCopySpecifics& specifics = {});
    void CopyToBuffer(Buffer& dst_buffer, const VkBufferCopy& copy_region);
    void CopyToImage(Image& dst_image, const BufferToImageCopySpecifics& specifics = {});
    void CopyToImage(Image& dst_image, const VkBufferImageCopy& copy_data, bool gen_mipmaps = false);

    static Buffer Create(VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkDeviceSize size);
    static Buffer Create(const BufferCreateInfo& info);

    operator const VkBuffer& () { return m_Buffer; }

  private:
    Ref<Device> r_Device;

    VkBuffer m_Buffer;
    VkDeviceSize m_Size;
    VkDeviceMemory m_Memory;
    VkBufferUsageFlags m_Usage;

    VkAccessFlags m_CurrentAccessMask = VK_ACCESS_NONE;
    VkPipelineStageFlags m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    bool m_Used = false;
  };

}