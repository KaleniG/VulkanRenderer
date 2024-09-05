#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  struct ImageToImageCopySpecifics
  {
    std::vector<VkImageCopy> CopyRegions = {};
    bool GenerateMipmaps = false;
  };

  struct ImageTransitionSpecifics
  {
    VkAccessFlags AccessMask = VK_ACCESS_NONE;
    VkPipelineStageFlags PipelineStagesMask = VK_PIPELINE_STAGE_NONE;
  };

  struct ImageToBufferCopySpecifics
  {
    std::vector<VkBufferImageCopy> CopyData = {};
    // RESERVED FOR FUTURE CHANGES
  };

  struct ImageCreateInfo
  {
    VkImageType Type;
    VkExtent3D Extent;
    VkFormat Format;
    VkImageUsageFlags Usage;
    VkMemoryPropertyFlags MemoryProperties;
    VkImageCreateFlags Flags = 0;
    VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
    uint32_t MipmapLevels = 1;
    uint32_t LayerCount = 1;
  };

  class Buffer; // Fuck includes
  class Image
  {
  public:
    ~Image();

    const VkImage& Get() const { return m_Image; }
    const VkDeviceMemory& GetMemory() const { return m_Memory; }
    const VkExtent3D& GetExtent() const { return m_Extent; }
    const VkImageAspectFlags& GetAspect() const { return m_Aspect; }
    const uint32_t& GetMipmapLevels() const { return m_MipmapLevels; }
    const uint32_t& GetLayerCount() const { return m_LayerCount; }
    const uint32_t& GetSize() const { return m_Size; }
    const VkImageUsageFlags& GetUsage() const { return m_Usage; }
    const VkImageType& GetType() const { return m_Type; }
    const VkImageLayout& GetLayout() const { return m_CurrentLayout; }

    void Transition(const VkImageLayout& new_layout, const ImageTransitionSpecifics& specifics = {});
    void CopyToImage(Image& dst_image, const ImageToImageCopySpecifics& specifics = {});
    void CopyToImage(Image& dst_image, const VkImageCopy& copy_region, bool gen_mipmaps = false);
    void CopyToBuffer(Buffer& dst_buffer, const ImageToBufferCopySpecifics& specifics = {});
    void CopyToBuffer(Buffer& dst_buffer, const VkBufferImageCopy& copy_data);

    static Image Create(VkImageType type, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkImageCreateFlags flags = 0, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT, uint32_t mipmap_levels = 1, uint32_t layer_count = 1);
    static Image Create(const ImageCreateInfo& info);

  private:
    Ref<Device> r_Device;

    VkImage m_Image;
    VkDeviceMemory m_Memory;
    VkFormat m_Format;
    VkImageType m_Type;
    VkImageUsageFlags m_Usage;
    VkExtent3D m_Extent;
    VkImageAspectFlags m_Aspect;
    uint32_t m_MipmapLevels;
    uint32_t m_LayerCount;
    uint32_t m_Size;

    VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkAccessFlags m_CurrentAccessMask = VK_ACCESS_NONE;
    VkPipelineStageFlags m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  };

}