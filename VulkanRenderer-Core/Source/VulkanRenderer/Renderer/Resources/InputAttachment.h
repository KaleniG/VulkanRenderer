#pragma once

#include "VulkanRenderer/Renderer/Resources/Attachment.h"

namespace vkren
{

  struct InputAttachmentCreateInfo
  {
    VkFormat Format;
    VkExtent2D Extent;
    VkComponentMapping ComponentMapping = DEFAULT_VIEW_COMPONENT_MAPPING;
    uint32_t LayerCount = 1;
    uint32_t MipmapLevels = 1;
    bool Copiable = false;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
  };

  class InputAttachment : public Attachment
  {
  public:
    static Ref<InputAttachment> Create(const InputAttachmentCreateInfo& info);
    static Ref<InputAttachment> Create
    (
      const VkFormat& format,
      const VkExtent2D& extent,
      const VkComponentMapping& view_component_mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
      uint32_t layer_count = 1,
      uint32_t mipmap_levels = 1,
      bool copiable = false,
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );

    // DELETES
    static Ref<Attachment> Create(const AttachmentCreateInfo& info) = delete;
    static Ref<Attachment> Create
    (
      const VkFormat& format,
      const AttachmentTypeFlags type,
      const VkExtent3D& extent,
      const VkComponentMapping& view_component_mapping = DEFAULT_VIEW_COMPONENT_MAPPING,
      uint32_t layer_count = 1,
      uint32_t mipmap_levels = 1,
      bool copiable = false,
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    ) = delete;
  };

}