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
    barrier.dstAccessMask = Utils::ImageLayoutToAccessMask(new_layout, specifics.AccessMask);

    Debug::AccessMaskToImageUsageCheck(m_Usage, barrier.dstAccessMask);

    VkPipelineStageFlags dstStages = Utils::AccessMaskToPipelineStagesMask(barrier.dstAccessMask, specifics.PipelineStagesMask);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();
    vkCmdPipelineBarrier(commandBuffer, m_CurrentPipelineStageMask, dstStages, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &barrier);
    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    m_CurrentLayout = new_layout;
    m_CurrentAccessMask = barrier.dstAccessMask;
    m_CurrentPipelineStageMask = dstStages;
  }

  Image Image::Create(const ImageCreateInfo& info)
  {
    Image image;

    image.r_Device = Renderer::GetDeviceRef();

    image.m_Format = info.Format;
    image.m_CurrentLayout = info.InitialLayout;
    image.m_Usage = info.Usage;

    switch (info.Format)
    {
      // Depth-only formats
      case VK_FORMAT_D16_UNORM:
      case VK_FORMAT_D32_SFLOAT:
      {
        image.m_Aspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
        break;
      }

      // Stencil-only formats
      case VK_FORMAT_S8_UINT:
      {
        image.m_Aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
        break;
      }

      // Depth-stencil formats
      case VK_FORMAT_D16_UNORM_S8_UINT:
      case VK_FORMAT_D32_SFLOAT_S8_UINT:
      {
        image.m_Aspect |= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        break;
      }

      // Color formats
      case VK_FORMAT_R8G8B8A8_UNORM:
      case VK_FORMAT_R8G8B8A8_SRGB:
      case VK_FORMAT_R8G8B8A8_UINT:
      case VK_FORMAT_R8G8B8A8_SNORM:
      case VK_FORMAT_R8G8B8A8_SINT:
      case VK_FORMAT_B8G8R8A8_UNORM:
      case VK_FORMAT_B8G8R8A8_SRGB:
      case VK_FORMAT_B8G8R8A8_UINT:
      case VK_FORMAT_B8G8R8A8_SNORM:
      case VK_FORMAT_B8G8R8A8_SINT:
      case VK_FORMAT_R16G16B16A16_SFLOAT:
      case VK_FORMAT_R16G16B16A16_UNORM:
      case VK_FORMAT_R16G16B16A16_SNORM:
      case VK_FORMAT_R16G16B16A16_UINT:
      case VK_FORMAT_R16G16B16A16_SINT:
      case VK_FORMAT_R32G32B32A32_SFLOAT:
      case VK_FORMAT_R32G32B32A32_UINT:
      case VK_FORMAT_R32G32B32A32_SINT:
      case VK_FORMAT_R32G32B32_SFLOAT:
      case VK_FORMAT_R32G32B32_UINT:
      case VK_FORMAT_R32G32B32_SINT:
      {
        image.m_Aspect |= VK_IMAGE_ASPECT_COLOR_BIT;
        break;
      }

      default:
      {
        CORE_ASSERT(false, "[VULKAN/SYSTEM] Invalid or not yet implemented image format specified");
        break;
      }
    }

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
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
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(image.r_Device->GetLogical(), image.m_Image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo = {};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = image.r_Device->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, info.MemoryProperties);

    result = vkAllocateMemory(image.r_Device->GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &image.m_Memory);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory to the image");

    result = vkBindImageMemory(image.r_Device->GetLogical(), image.m_Image, image.m_Memory, 0);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the image memoey");

    return image;
  }

}