#pragma once

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/ViewImage.h"

namespace vkren
{

  enum AttachmentTypeFlagBits
  {
    VKREN_ATTACHMENT_TYPE_COLOR_BIT = 0x00000001,
    VKREN_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT = 0x00000002,
    VKREN_ATTACHMENT_TYPE_RESOLVE_BIT = 0x00000004,
    VKREN_ATTACHMENT_TYPE_INPUT_BIT = 0x00000008,
    VKREN_ATTACHMENT_TYPE_TRANSIENT_BIT = 0x00000010
  };

  typedef uint32_t AttachmentTypeFlags;

  struct AttachmentCreateInfo
  {
    VkFormat Format;
    AttachmentTypeFlags Type;
    VkExtent2D Extent;
    VkComponentMapping ComponentMapping = DEFAULT_VIEW_COMPONENT_MAPPING;
    uint32_t LayerCount = 1;
    uint32_t MipmapLevels = 1;
    bool Copiable = false;
    VkSampleCountFlagBits SampleCount = VK_SAMPLE_COUNT_1_BIT;
  };

  class Attachment : public ViewImage
  {
  public:
    const AttachmentTypeFlags& GetAttachmentType() const { return m_AttachmentType; }

    static Ref<Attachment> Create(const AttachmentCreateInfo& info);
    static Ref<Attachment> Create
    (
      const VkFormat& format,
      const AttachmentTypeFlags type,
      const VkExtent2D& extent,
      const VkComponentMapping& view_component_mapping = DEFAULT_VIEW_COMPONENT_MAPPING,
      uint32_t layer_count = 1,
      uint32_t mipmap_levels = 1,
      bool copiable = false,
      const VkSampleCountFlagBits& sample_count = VK_SAMPLE_COUNT_1_BIT
    );

  protected:
    void CreateAttachment(const VkFormat& format, const AttachmentTypeFlags type, const VkExtent2D& extent, uint32_t layer_count, uint32_t mipmap_levels, bool copiable, const VkSampleCountFlagBits& sample_count);

  protected:
    AttachmentTypeFlags m_AttachmentType;
  };

}