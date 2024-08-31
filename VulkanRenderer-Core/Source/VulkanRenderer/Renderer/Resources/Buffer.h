#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Resources/Image.h"

namespace vkren
{

  struct BufferTransitionSpecifics
  {
    VkPipelineStageFlags PipelineStagesMask = VK_PIPELINE_STAGE_NONE;
  };

  class Buffer
  {
  public:
    ~Buffer();

    const VkBuffer& Get() const { return m_Buffer; }
    const VkDeviceSize& GetSize() const { return m_Size; }
    const VkAccessFlags& GetAccessMask() const { return m_CurrentAccessMask; } 
    const VkBufferUsageFlags& GetUsage() const { return m_Usage; }
    const VkDeviceMemory& GetMemory() const { return m_Memory; }

    void SetAccessMask(const VkAccessFlags& mask) { m_CurrentAccessMask = mask; }
    void SetPipelineStageMask(const VkPipelineStageFlags& mask) { m_CurrentPipelineStageMask = mask; }

    bool IsUsed() { return m_Used; }

    void Transition(const VkAccessFlags& new_access, const BufferTransitionSpecifics& specifics = {});
    void CopyToBuffer(Buffer& dst_buffer, const std::vector<VkBufferCopy>& copy_regions = {});
    void CopyToImage(Image& dst_image, const std::vector<VkBufferImageCopy>& copy_regions = {});

    static Buffer Create(VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkDeviceSize size);

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