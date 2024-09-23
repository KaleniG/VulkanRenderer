#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Image.h"

namespace vkren
{

  struct InputAttachmentCreateInfo
  {
    VkFormat Format;
    VkExtent2D Extent;
    VkComponentMapping ComponentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    uint32_t LayerCount = 1;
    bool Copiable = false;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
  };

  class InputAttachment : public Image
  {
  public:
    ~InputAttachment();

    const VkImageView& GetView() const { return m_View; }

    static InputAttachment Create(const InputAttachmentCreateInfo& info);
    static InputAttachment Create
    (
      const VkFormat& format,
      const VkExtent2D& extent,
      const VkComponentMapping& view_component_mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
      const uint32_t& layer_count = 1,
      bool copiable = false,
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );

  private:
    VkImageView m_View = VK_NULL_HANDLE;
  };

}