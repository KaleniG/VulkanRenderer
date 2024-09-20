#pragma once

#include "VulkanRenderer/Core/Base.h"

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Image.h"
#include "VulkanRenderer/Renderer/Resources/Sampler.h"

namespace vkren
{

  static struct SamplerConfig
  {
    Ref<Sampler> Sampler = nullptr;
    SamplerCreateInfo SamplerCreateInfo = {};
  };

  struct SampledImageCreateInfo
  {
    ImageCreateInfo Image = {};
    ImageViewCreateInfo View = {};
    SamplerConfig Sampler = {};
  };

  class SampledImage : public Image
  {
  public:
    ~SampledImage();

    const Ref<Sampler>& GetSampler() const { return m_Sampler; }
    const VkImageView& GetView() const { return m_View; }

    static SampledImage Create(const SampledImageCreateInfo& info);
    static SampledImage Create
    (
      const VkFormat& format, 
      const VkImageType& type,
      const VkExtent3D& extent, 
      const Ref<Sampler>& sampler = nullptr,
      const SamplerCreateInfo& sampler_create_info = {}, 
      const VkComponentMapping& view_component_mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY }, 
      const uint32_t& layer_count = 1, 
      const uint32_t& mipmap_levels = 1, 
      bool copiable = false, 
      const VkImageCreateFlags& flags = 0, 
      const VkImageTiling& tiling = VK_IMAGE_TILING_OPTIMAL, 
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );

  private:
    Ref<Sampler> m_Sampler;
    VkImageView m_View = VK_NULL_HANDLE;
  };

}