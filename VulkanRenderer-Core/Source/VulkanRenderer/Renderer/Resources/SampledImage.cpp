#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/SampledImage.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  SampledImage SampledImage::Create(const SampledImageCreateInfo& info)
  {
    return SampledImage::Create(info.Format, info.Type, info.Extent, info.Sampler.Sampler, info.Sampler.SamplerCreateInfo, info.ComponentMapping, info.LayerCount, info.MipmapLevels, info.Copiable, info.Flags, info.SampleCount);
  }

  SampledImage SampledImage::Create(const VkFormat& format, const VkImageType& type, const VkExtent3D& extent, const Ref<Sampler>& sampler, const SamplerCreateInfo& sampler_create_info, const VkComponentMapping& view_component_mapping, const uint32_t& layer_count, const uint32_t& mipmap_levels, bool copiable, const VkImageCreateFlags& flags, const VkSampleCountFlagBits& sample_count)
  {
    SampledImage image;
    image.CreateImage(format, type, extent, (copiable ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0) | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layer_count, mipmap_levels, flags, VK_IMAGE_TILING_OPTIMAL, sample_count);
    
    if (sampler)
      image.r_Sampler = sampler;
    else
    {
      SamplerCreateInfo createInfo = sampler_create_info;
      createInfo.ImageFormat = image.m_Format;
      createInfo.ImageTiling = image.m_Tiling;
      image.r_Sampler = Sampler::Create(createInfo);
    }

    if (image.r_Sampler->UsesLinearFiltering())
      CORE_ASSERT(Renderer::GetDevice().GetFormatProperties(format).optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT, "[VULKAN/SYSTEM] The specified format for this image does not support linear tiling");

    image.CreateImageView(view_component_mapping);

    return image;
  }

}