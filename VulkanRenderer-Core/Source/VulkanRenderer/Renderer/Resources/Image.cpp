#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/Image.h"
#include "VulkanRenderer/Renderer/Utils/Functions.h"
#include "VulkanRenderer/Renderer/Utils/Debug.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  Image::~Image()
  {
    vkDestroyImage(r_Device->GetLogical(), m_Image, VK_NULL_HANDLE);
    vkFreeMemory(r_Device->GetLogical(), m_Memory, VK_NULL_HANDLE);
  }

  void Image::Transition(const VkImageLayout& new_layout, const ImageTransitionSpecifics& specifics)
  {
    if (m_CurrentLayout == new_layout)
    {
      CORE_WARN("[VULKAN/SYSTEM] Transitioning the image to the same layout it is at the moment");
      return;
    }

    Debug::ImageLayoutToAspectCheck(new_layout, m_Aspect);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = m_CurrentLayout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
    barrier.subresourceRange.aspectMask = m_Aspect;
    barrier.srcAccessMask = m_CurrentAccessMask;
    barrier.dstAccessMask = Utils::VkImageLayoutToVkAccessMask(new_layout, specifics.AccessMask);

    Debug::AccessMaskToImageUsageCheck(m_Usage, barrier.dstAccessMask);

    VkPipelineStageFlags dstStages = Utils::VkAccessMaskToVkPipelineStagesMask(barrier.dstAccessMask, specifics.PipelineStagesMask);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();
    vkCmdPipelineBarrier(commandBuffer, m_CurrentPipelineStageMask, dstStages, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &barrier);
    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    m_CurrentLayout = new_layout;
    m_CurrentAccessMask = barrier.dstAccessMask;
    m_CurrentPipelineStageMask = dstStages;
  }

  Image Image::Create(const ImageCreateInfo& info)
  {
    switch (info.Type)
    {
      case VK_IMAGE_TYPE_1D:
      {
        CORE_ASSERT(info.Extent.height == 1 || info.Extent.depth == 1, "[VULKAN/SYSTEM] Invalid height/depth specified for a 1D Image");
        break;
      }
      case VK_IMAGE_TYPE_2D:
      {
        CORE_ASSERT(info.Extent.depth == 1, "[VULKAN/SYSTEM] Invalid depth specified for a 2D Image");
        if (info.Flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
          CORE_ASSERT(!(info.LayerCount % 6), "[VULKAN/SYSTEM] If the image is a cubemap, the layer count should be a multiple of 6");
        break;
      }
      case VK_IMAGE_TYPE_3D:
      {
        CORE_ASSERT(info.LayerCount == 1, "[VULKAN/SYSTEM] Invalid layer count specified for a 3D Image");
        break;
      }
      default:
      {
        CORE_ASSERT(false, "[VULKAN/SYSTEM] Unsupported image type specified");
        break;
      }
    }

    Image image;

    image.r_Device = Renderer::GetDeviceRef();

    image.m_Format = info.Format;
    image.m_CurrentLayout = info.InitialLayout;
    image.m_Usage = info.Usage;
    image.m_Extent = info.Extent;
    image.m_MipmapLevels = info.MipmapLevels;
    image.m_LayerCount = info.LayerCount;
    image.m_Type = info.Type;

    image.m_Aspect = Utils::VkFormatToVkAspectMask(info.Format);

    image.m_Size = 0;

    for (uint32_t layer = 0; layer < info.LayerCount; layer++)
    {
      uint32_t mipWidth = info.Extent.width;
      uint32_t mipHeight = info.Extent.height;
      uint32_t mipDepth = info.Extent.depth;

      for (uint32_t mip = 0; mip < info.MipmapLevels; mip++)
      {
        uint32_t mipLevelSize = Utils::VkFormatToByteSize(info.Format) * mipWidth * mipHeight * mipDepth;
        image.m_Size += mipLevelSize;

        mipWidth = std::max(1u, mipWidth / 2);
        mipHeight = std::max(1u, mipHeight / 2);
        mipDepth = std::max(1u, mipDepth / 2);
      }
    }

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.flags = info.Flags;
    imageCreateInfo.imageType = info.Type;
    imageCreateInfo.extent = info.Extent;
    imageCreateInfo.mipLevels = info.MipmapLevels;
    imageCreateInfo.arrayLayers = info.LayerCount;
    imageCreateInfo.format = info.Format;
    imageCreateInfo.tiling = info.Tiling;
    imageCreateInfo.initialLayout = info.InitialLayout;
    imageCreateInfo.usage = info.Usage;
    imageCreateInfo.samples = info.SampleCount; // TO RETHINK
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(image.r_Device->GetLogical(), &imageCreateInfo, VK_NULL_HANDLE, &image.m_Image);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image. {}", Utils::VkResultToString(result));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(image.r_Device->GetLogical(), image.m_Image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo = {};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = image.r_Device->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, info.MemoryProperties);

    result = vkAllocateMemory(image.r_Device->GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &image.m_Memory);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory for the image. {}", Utils::VkResultToString(result));

    result = vkBindImageMemory(image.r_Device->GetLogical(), image.m_Image, image.m_Memory, 0);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the image memory. {}", Utils::VkResultToString(result));

    return image;
  }

}
