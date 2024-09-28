#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/InputAttachment.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{
  Ref<InputAttachment> InputAttachment::Create(const InputAttachmentCreateInfo& info)
  {
    return InputAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.LayerCount, info.MipmapLevels, info.Copiable, info.SampleCount);
  }

  Ref<InputAttachment> InputAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, uint32_t layer_count, uint32_t mipmap_levels, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    Ref<InputAttachment> attachment = CreateRef<InputAttachment>();

    attachment->CreateAttachment(format, VKREN_ATTACHMENT_TYPE_INPUT_BIT, extent, layer_count, mipmap_levels, copiable, sample_count);
    attachment->CreateImageView(view_component_mapping);

    return attachment;
  }

}