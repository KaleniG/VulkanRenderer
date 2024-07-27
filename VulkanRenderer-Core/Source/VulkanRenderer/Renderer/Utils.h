#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{
  namespace vkutils
  {
    
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
    {
      VkImageViewCreateInfo imageViewCreateInfo{};
      imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      imageViewCreateInfo.image = image;
      imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      imageViewCreateInfo.format = format;
      imageViewCreateInfo.subresourceRange.aspectMask = aspect_flags;
      imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
      imageViewCreateInfo.subresourceRange.levelCount = 1;
      imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
      imageViewCreateInfo.subresourceRange.layerCount = 1;
      imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

      VkImageView imageView;
      VkResult result = vkCreateImageView(Renderer::GetDevice().GetLogical(), &imageViewCreateInfo, VK_NULL_HANDLE, &imageView);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image view");

      return imageView;
    }

    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory)
    {
      VkImageCreateInfo imageCreateInfo{};
      imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
      imageCreateInfo.extent.width = width;
      imageCreateInfo.extent.height = height;
      imageCreateInfo.extent.depth = 1;
      imageCreateInfo.mipLevels = 1;
      imageCreateInfo.arrayLayers = 1;
      imageCreateInfo.format = format;
      imageCreateInfo.tiling = tiling;
      imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageCreateInfo.usage = usage;
      imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      VkResult result = vkCreateImage(Renderer::GetDevice().GetLogical(), &imageCreateInfo, VK_NULL_HANDLE, &image);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image");

      VkMemoryRequirements memoryRequirements;
      vkGetImageMemoryRequirements(Renderer::GetDevice().GetLogical(), image, &memoryRequirements);

      VkMemoryAllocateInfo memoryAllocInfo{};
      memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memoryAllocInfo.allocationSize = memoryRequirements.size;
      memoryAllocInfo.memoryTypeIndex = Renderer::GetDevice().FindMemoryType(memoryRequirements.memoryTypeBits, properties);

      result = vkAllocateMemory(Renderer::GetDevice().GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &image_memory);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory to the image");

      result = vkBindImageMemory(Renderer::GetDevice().GetLogical(), image, image_memory, 0);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the image memory");
    }
    
  }
}