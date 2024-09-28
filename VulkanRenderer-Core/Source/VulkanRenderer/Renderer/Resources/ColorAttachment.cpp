#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/ColorAttachment.h"

namespace vkren
{

  Ref<ColorAttachment> ColorAttachment::Create(const ColorAttachmentCreateInfo& info)
  {
    return ColorAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.Copiable, info.SampleCount);
  }

  Ref<ColorAttachment> ColorAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    Ref<ColorAttachment> attachment = CreateRef<ColorAttachment>();

    attachment->CreateAttachment(format, VKREN_ATTACHMENT_TYPE_COLOR_BIT, extent, 1, 1, copiable, sample_count);
    attachment->CreateImageView(view_component_mapping);

    return attachment;
  }

}