#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/SampledImage.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  SampledImage::~SampledImage()
  {
    vkDestroyImageView(Renderer::GetDevice().GetLogical(), m_View, VK_NULL_HANDLE);
  }

  SampledImage SampledImage::Create(const SampledImageCreateInfo& info)
  {
    return SampledImage::Create(info.Format, info.Type, info.Extent, info.Sampler.Sampler, info.Sampler.SamplerCreateInfo, info.ComponentMapping, info.LayerCount, info.MipmapLevels, info.Copiable, info.Flags, info.SampleCount);
  }

  SampledImage SampledImage::Create(const VkFormat& format, const VkImageType& type, const VkExtent3D& extent, const Ref<Sampler>& sampler, const SamplerCreateInfo& sampler_create_info, const VkComponentMapping& view_component_mapping, const uint32_t& layer_count, const uint32_t& mipmap_levels, bool copiable, const VkImageCreateFlags& flags, const VkSampleCountFlagBits& sample_count)
  {
    SampledImage image;
    image.CreateImage(format, type, extent, (copiable ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0) | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layer_count, mipmap_levels, flags, VK_IMAGE_TILING_OPTIMAL, sample_count);
    
    if (sampler)
      image.m_Sampler = sampler;
    else
    {
      SamplerCreateInfo createInfo = sampler_create_info;
      createInfo.ImageFormat = image.m_Format;
      createInfo.ImageTiling = image.m_Tiling;
      image.m_Sampler = Sampler::Create(createInfo);
    }

    if (image.m_Sampler->UsesLinearFiltering())
      CORE_ASSERT(Renderer::GetDevice().GetFormatProperties(format).optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT, "[VULKAN/SYSTEM] The specified format for this image does not support linear tiling");

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image.m_Image;
    imageViewCreateInfo.format = image.m_Format;
    imageViewCreateInfo.subresourceRange.aspectMask = image.m_Aspect;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = image.m_MipmapLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = image.m_LayerCount;
    imageViewCreateInfo.components = view_component_mapping;

    switch (image.m_Type)
    {
      case VK_IMAGE_TYPE_1D:
      {
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
        if (image.m_LayerCount > 1)
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        break;
      }
      case VK_IMAGE_TYPE_2D:
      {
        if (image.m_CreateFlags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
        {
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
          if (image.m_LayerCount > 6)
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
          break;
        }
        else
        {
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
          if (image.m_LayerCount > 1)
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
          break;
        }
      }
      case VK_IMAGE_TYPE_3D:
      {
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
      }
    }

    VkResult result = vkCreateImageView(Renderer::GetDevice().GetLogical(), &imageViewCreateInfo, VK_NULL_HANDLE, &image.m_View);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image view. {}", Utils::VkResultToString(result));

    return image;
  }

}