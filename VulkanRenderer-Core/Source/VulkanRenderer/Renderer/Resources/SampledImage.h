#pragma once

#include "VulkanRenderer/Renderer/Resources/Sampler.h"
#include "VulkanRenderer/Renderer/Resources/Image.h"

namespace vkren
{

  static struct ImageConfig
  {
    VkFormat Format;
    VkImageType Type;
    VkExtent3D Extent;

    uint32_t LayerCount = 1;
    uint32_t MipmapLevels = 1;
    bool Copiable = false;
    VkImageCreateFlags Flags = 0;
    VkImageTiling Tiling = VK_IMAGE_TILING_OPTIMAL;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
  };

  static struct SamplerConfig
  {
    Ref<Sampler> Sampler = nullptr;
    SamplerCreateInfo SamplerCreateInfo = {};
  };

  static struct ViewConfig
  {
    VkComponentMapping ViewComponentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
  };

  struct SampledImageCreateInfo
  {
    ImageConfig Image = {};
    SamplerConfig Sampler = {};
    ViewConfig View = {};
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