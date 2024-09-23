#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/InputAttachment.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  InputAttachment::~InputAttachment()
  {
    vkDestroyImageView(Renderer::GetDevice().GetLogical(), m_View, VK_NULL_HANDLE);
  }

  InputAttachment InputAttachment::Create(const InputAttachmentCreateInfo& info)
  {
    return InputAttachment::Create(info.Format, info.Extent, info.ComponentMapping, info.LayerCount, info.Copiable, info.SampleCount);
  }

  InputAttachment InputAttachment::Create(const VkFormat& format, const VkExtent2D& extent, const VkComponentMapping& view_component_mapping, const uint32_t& layer_count, bool copiable, const VkSampleCountFlagBits& sample_count)
  {
    InputAttachment attachment;
    attachment.CreateImage(format, VK_IMAGE_TYPE_2D, { extent.width, extent.height, 1 }, (copiable ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0) | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layer_count, 1, 0, VK_IMAGE_TILING_OPTIMAL, sample_count);

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = attachment.m_Image;
    imageViewCreateInfo.format = attachment.m_Format;
    imageViewCreateInfo.subresourceRange.aspectMask = attachment.m_Aspect;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = attachment.m_MipmapLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = attachment.m_LayerCount;
    imageViewCreateInfo.components = view_component_mapping;

    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    if (attachment.m_LayerCount > 1)
      imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

    VkResult result = vkCreateImageView(Renderer::GetDevice().GetLogical(), &imageViewCreateInfo, VK_NULL_HANDLE, &attachment.m_View);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image view. {}", Utils::VkResultToString(result));

    return attachment;
  }

}