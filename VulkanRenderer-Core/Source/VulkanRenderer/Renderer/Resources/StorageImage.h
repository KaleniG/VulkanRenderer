#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Image.h"

namespace vkren
{

  struct StorageImageCreateInfo
  {
    ImageCreateInfo Image = {};
    ImageViewCreateInfo View = {};
  };

  class StorageImage : public Image
  {
  public:
    ~StorageImage();

    const VkImageView& GetView() const { return m_View; }

    static StorageImage Create(const StorageImageCreateInfo& info);
    static StorageImage Create
    (
      const VkFormat& format, 
      const VkImageType& type, 
      const VkExtent3D& extent, 
      const VkComponentMapping& view_component_mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }, 
      const uint32_t& layer_count = 1, 
      const uint32_t& mipmap_levels = 1, 
      bool copiable = false, 
      const VkImageCreateFlags& flags = 0, 
      const VkImageTiling& tiling = VK_IMAGE_TILING_OPTIMAL, 
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );

  private:
    VkImageView m_View = VK_NULL_HANDLE;
  };

}