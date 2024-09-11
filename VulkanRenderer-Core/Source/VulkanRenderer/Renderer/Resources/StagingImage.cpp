#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/StagingImage.h"

namespace vkren
{

  StagingImage StagingImage::Create(Image& src_image, bool copy)
  {
    StagingImage& src_image_temp = static_cast<StagingImage&>(src_image);
    StagingImage image = StagingImage::Create(src_image_temp.m_Format, src_image_temp.m_Type, src_image_temp.m_Extent, src_image_temp.m_LayerCount, src_image_temp.m_MipmapLevels, src_image_temp.m_CreateFlags, src_image_temp.m_Tiling, src_image_temp.m_SampleCount);

    if (copy)
      src_image.CopyToImage(static_cast<Image&>(image));

    return image;
  }

  StagingImage StagingImage::Create(const StagingImageCreateInfo& info)
  {
    return StagingImage::Create(info.Format, info.Type, info.Extent, info.LayerCount, info.MipmapLevels, info.Flags, info.Tiling, info.SampleCount);
  }

  StagingImage StagingImage::Create(const VkFormat& format, const VkImageType& type, const VkExtent3D& extent, const uint32_t& layer_count, const uint32_t& mipmap_levels, const VkImageCreateFlags& flags, const VkImageTiling& tiling, const VkSampleCountFlagBits& sample_count)
  {
    StagingImage image; 
    image.CreateImage(format, type, extent, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layer_count, mipmap_levels, flags, tiling, sample_count);
    return image;
  }

}