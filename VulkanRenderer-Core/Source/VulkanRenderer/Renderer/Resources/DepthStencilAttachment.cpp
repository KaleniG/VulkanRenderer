#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/DepthStencilAttachment.h"

namespace vkren
{

  Ref<DepthStencilAttachment> DepthStencilAttachment::Create(const DepthStencilAttachmentCreateInfo& info)
  {
    return DepthStencilAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.Copiable, info.SampleCount);
  }

  Ref<DepthStencilAttachment> DepthStencilAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    Ref<DepthStencilAttachment> attachment = CreateRef<DepthStencilAttachment>();

    attachment->CreateAttachment(format, VKREN_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT, extent, 1, 1, copiable, sample_count);
    attachment->CreateImageView(view_component_mapping);

    return attachment;
  }

}