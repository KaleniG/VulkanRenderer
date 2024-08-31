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

    void Transition(const VkAccessFlags& new_access, const BufferTransitionSpecifics& specifics = {}); // EXPERIMENTAL
    void CopyToBuffer(const Buffer& dst_buffer, const std::vector<VkBufferCopy>& copy_regions = {});
    //void CopyToImage(const Image& dst_image, const std::vector<VkBufferCopy>& copy_regions = {});

    static Buffer Create(VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkDeviceSize size);

    operator const VkBuffer& () { return m_Buffer; }

  private:
    Ref<Device> r_Device;

    VkBuffer m_Buffer;
    VkDeviceSize m_Size;
    VkBufferUsageFlags m_Usage;
    VkDeviceMemory m_Memory;

    VkAccessFlags m_CurrentAccessMask = VK_ACCESS_NONE;
    VkPipelineStageFlags m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    bool m_Used = false;
  };

}