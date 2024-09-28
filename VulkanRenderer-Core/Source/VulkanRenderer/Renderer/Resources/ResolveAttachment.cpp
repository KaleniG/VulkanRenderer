#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/ResolveAttachment.h"

namespace vkren
{

  Ref<ResolveAttachment> ResolveAttachment::Create(const ResolveAttachmentCreateInfo& info)
  {
    return ResolveAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.Copiable);
  }

  Ref<ResolveAttachment> ResolveAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, bool copiable)
  {
    Ref<ResolveAttachment> attachment = CreateRef<ResolveAttachment>();

    attachment->CreateAttachment(format, VKREN_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT, extent, 1, 1, copiable, VK_SAMPLE_COUNT_1_BIT);
    attachment->CreateImageView(view_component_mapping);

    return attachment;
  }

}