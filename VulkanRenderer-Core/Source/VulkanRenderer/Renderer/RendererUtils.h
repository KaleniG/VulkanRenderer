#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"

namespace vkren
{
  namespace utils
  {
    VkImageView CreateImageView(VkDevice logical_device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
    {
      VkImageViewCreateInfo image_view_create_info;
      image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      image_view_create_info.image = image;
      image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      image_view_create_info.format = format;
      image_view_create_info.subresourceRange.aspectMask = aspect_flags;
      image_view_create_info.subresourceRange.baseMipLevel = 0;
      image_view_create_info.subresourceRange.levelCount = 1;
      image_view_create_info.subresourceRange.baseArrayLayer = 0;
      image_view_create_info.subresourceRange.layerCount = 1;
      image_view_create_info.flags = 0;
      image_view_create_info.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
      image_view_create_info.pNext = nullptr;

      VkImageView image_view;
      VkResult result = vkCreateImageView(logical_device, &image_view_create_info, nullptr, &image_view);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image view");

      return image_view;
    }
  }
}