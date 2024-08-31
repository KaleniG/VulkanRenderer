#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  struct ImageCreateInfo
  {
    VkImageType Type;
    VkExtent3D Extent;
    VkFormat Format;
    VkImageUsageFlags Usage;
    VkMemoryPropertyFlags MemoryProperties;
    VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
    uint32_t MipmapLevels = 1;
    uint32_t LayerCount = 1;
    VkImageLayout InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  };

  struct ImageTransitionSpecifics
  {
    VkAccessFlags AccessMask = VK_ACCESS_NONE;
    VkPipelineStageFlags PipelineStagesMask = VK_PIPELINE_STAGE_NONE;
  };

  class Image
  {
  public:
    ~Image();

    const VkImage& Get() const { return m_Image; }
    const VkDeviceMemory& GetMemory() const { return m_Memory; }

    void Transition(const VkImageLayout& new_layout, const ImageTransitionSpecifics& specifics = {});

    static Image Create(const ImageCreateInfo& info);

  private:

  private:
    Ref<Device> r_Device;

    VkImage m_Image;
    VkDeviceMemory m_Memory;
    VkFormat m_Format;
    VkImageUsageFlags m_Usage;
    VkImageAspectFlags m_Aspect = VK_IMAGE_ASPECT_NONE;

    VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkAccessFlags m_CurrentAccessMask = VK_ACCESS_NONE;
    VkPipelineStageFlags m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    bool m_Used = false;
  };

}