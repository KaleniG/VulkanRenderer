#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/ViewImage.h"
#include "VulkanRenderer/Renderer/Resources/Sampler.h"

namespace vkren
{

  struct SamplerConfig
  {
    Ref<Sampler> Sampler = nullptr;
    SamplerCreateInfo SamplerCreateInfo = {};
  };

  struct SampledImageCreateInfo
  {
    VkFormat Format;
    VkImageType Type;
    VkExtent3D Extent;
    VkComponentMapping ComponentMapping = DEFAULT_VIEW_COMPONENT_MAPPING;
    uint32_t LayerCount = 1;
    uint32_t MipmapLevels = 1;
    bool Copiable = false;
    VkImageCreateFlags Flags = 0;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
    SamplerConfig Sampler = {};
  };

  class SampledImage : public ViewImage
  {
  public:
    const Ref<Sampler>& GetSampler() const { return r_Sampler; }

    static Ref<SampledImage> Create(const SampledImageCreateInfo& info);
    static Ref<SampledImage> Create
    (
      const VkFormat& format, 
      const VkImageType& type,
      const VkExtent3D& extent, 
      const Ref<Sampler>& sampler = nullptr,
      const SamplerCreateInfo& sampler_create_info = {}, 
      const VkComponentMapping& view_component_mapping = DEFAULT_VIEW_COMPONENT_MAPPING,
      const uint32_t& layer_count = 1, 
      const uint32_t& mipmap_levels = 1, 
      bool copiable = false, 
      const VkImageCreateFlags& flags = 0,
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );

  private:
    Ref<Sampler> r_Sampler;
  };

}