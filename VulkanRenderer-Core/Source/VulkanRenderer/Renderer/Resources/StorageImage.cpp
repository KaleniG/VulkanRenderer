#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/StorageImage.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  StorageImage::~StorageImage()
  {
    vkDestroyImageView(Renderer::GetDevice().GetLogical(), m_View, VK_NULL_HANDLE);
  }

  StorageImage StorageImage::Create(const StorageImageCreateInfo& info)
  {
    return StorageImage::Create(info.Format, info.Type, info.Extent, info.ComponentMapping, info.LayerCount, info.MipmapLevels, info.Copiable, info.Flags, info.SampleCount);
  }

  StorageImage StorageImage::Create(const VkFormat& format, const VkImageType& type, const VkExtent3D& extent, const VkComponentMapping& view_component_mapping, const uint32_t& layer_count, const uint32_t& mipmap_levels, bool copiable, const VkImageCreateFlags& flags, const VkSampleCountFlagBits& sample_count)
  {
    StorageImage image;
    image.CreateImage(format, type, extent, (copiable ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0) | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, layer_count, mipmap_levels, flags, VK_IMAGE_TILING_OPTIMAL, sample_count);

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image.m_Image;
    imageViewCreateInfo.format = image.m_Format;
    imageViewCreateInfo.subresourceRange.aspectMask = image.m_Aspect;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = image.m_MipmapLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = image.m_LayerCount;
    imageViewCreateInfo.components = view_component_mapping;

    switch (image.m_Type)
    {
      case VK_IMAGE_TYPE_1D:
      {
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
        if (image.m_LayerCount > 1)
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        break;
      }
      case VK_IMAGE_TYPE_2D:
      {
        if (image.m_CreateFlags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
        {
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
          if (image.m_LayerCount > 6)
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
          break;
        }
        else
        {
          imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
          if (image.m_LayerCount > 1)
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
          break;
        }
      }
      case VK_IMAGE_TYPE_3D:
      {
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
      }
    }

    VkResult result = vkCreateImageView(Renderer::GetDevice().GetLogical(), &imageViewCreateInfo, VK_NULL_HANDLE, &image.m_View);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image view. {}", Utils::VkResultToString(result));

    return image;
  }

}