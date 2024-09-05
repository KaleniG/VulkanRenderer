#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/Buffer.h"
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

    CORE_INFO("[SYSTEM] Image '{}' transitioned", (int)m_Image);
  }

  void Image::CopyToImage(Image& dst_image, const ImageToImageCopySpecifics& specifics)
  {
    CORE_ASSERT(m_Usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT, "[VULKAN/SYSTEM] The source image cannot be used as a data transfer source, 'VK_IMAGE_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its image creation");
    CORE_ASSERT(dst_image.GetUsage() & VK_IMAGE_USAGE_TRANSFER_DST_BIT, "[VULKAN/SYSTEM] The destination image cannot be used as a data transfer destination, 'VK_IMAGE_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during the image creation");

    if (m_CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
      Transition(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    if (dst_image.GetLayout() != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
      dst_image.Transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();
    if (specifics.CopyRegions.size())
    {
      for (const VkImageCopy& copyRegion : specifics.CopyRegions)
      {
        CORE_ASSERT(copyRegion.srcOffset.x + copyRegion.extent.width <= m_Extent.width && copyRegion.srcOffset.y + copyRegion.extent.height <= m_Extent.height && copyRegion.srcOffset.z + copyRegion.extent.depth <= m_Extent.depth, "[VULKAN/SYSTEM] Copying off-range data from the source image");
        CORE_ASSERT(copyRegion.dstOffset.x + copyRegion.extent.width <= dst_image.GetExtent().width && copyRegion.dstOffset.y + copyRegion.extent.height <= dst_image.GetExtent().height && copyRegion.dstOffset.z + copyRegion.extent.depth <= dst_image.GetExtent().depth, "[VULKAN/SYSTEM] Copying to an area that is out of range of the destination image");
        CORE_ASSERT(copyRegion.srcSubresource.baseArrayLayer + copyRegion.srcSubresource.layerCount <= m_LayerCount, "[VULKAN/SYSTEM] Copying more layers than available in the source image");
        CORE_ASSERT(copyRegion.dstSubresource.baseArrayLayer + copyRegion.dstSubresource.layerCount <= dst_image.GetLayerCount(), "[VULKAN/SYSTEM] Copying more layers than available in the destination image");
        CORE_ASSERT(copyRegion.srcSubresource.mipLevel <= m_MipmapLevels, "[VULKAN/SYSTEM] Specified mip level exceeds the available mip levels in the source image");
        CORE_ASSERT(copyRegion.dstSubresource.mipLevel <= dst_image.GetMipmapLevels(), "[VULKAN/SYSTEM] Specified mip level exceeds the available mip levels in the destination image");
      }

      vkCmdCopyImage(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_image.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(specifics.CopyRegions.size()), specifics.CopyRegions.data());
    }
    else
    {
      CORE_ASSERT(m_Extent.width <= dst_image.GetExtent().width && m_Extent.height <= dst_image.GetExtent().height && m_Extent.depth <= dst_image.GetExtent().depth, "[VULKAN/SYSTEM] The source image size is greater than the destination image's size during whole image copy");
      if (m_Extent.width < dst_image.GetExtent().width || m_Extent.height < dst_image.GetExtent().height || m_Extent.depth < dst_image.GetExtent().depth)
        CORE_WARN("[VULKAN/SYSTEM] Executing complete copy of a source image (size:{0}x{1}x{2}) to a destination image with bigger size (size:{3}x{4}x{5})", m_Extent.width, m_Extent.height, m_Extent.depth, dst_image.GetExtent().width, dst_image.GetExtent().height, dst_image.GetExtent().depth);

      VkImageCopy region = {};
      region.srcSubresource.aspectMask = m_Aspect;
      region.srcSubresource.baseArrayLayer = 0;
      region.srcSubresource.layerCount = m_LayerCount;
      region.srcSubresource.mipLevel = 0;
      region.dstSubresource.aspectMask = dst_image.GetAspect();
      region.dstSubresource.baseArrayLayer = 0;
      region.dstSubresource.layerCount = dst_image.GetLayerCount();
      region.dstSubresource.mipLevel = 0;
      region.srcOffset = { 0, 0, 0 };
      region.dstOffset = { 0, 0, 0 };
      region.extent = m_Extent;

      vkCmdCopyImage(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_image.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    CORE_INFO("[SYSTEM] Image '{}' copied to image '{}'", (int)m_Image, (int)dst_image.Get());
  }

  void Image::CopyToImage(Image& dst_image, const VkImageCopy& copy_region, bool gen_mipmaps)
  {
    ImageToImageCopySpecifics specifics = {};
    specifics.CopyRegions = { copy_region };
    specifics.GenerateMipmaps = gen_mipmaps;

    Image::CopyToImage(dst_image, specifics);
  }

  void Image::CopyToBuffer(Buffer& dst_buffer, const ImageToBufferCopySpecifics& specifics)
  {
    CORE_ASSERT(m_Usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT, "[VULKAN/SYSTEM] The source image cannot be used as a data transfer source, 'VK_IMAGE_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its image creation");
    CORE_ASSERT(dst_buffer.GetUsage() & VK_BUFFER_USAGE_TRANSFER_DST_BIT, "[VULKAN/SYSTEM] The destination buffer cannot be used as a data transfer destination, 'VK_BUFFER_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during buffer creation");

    if (m_CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
      Transition(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    if (dst_buffer.GetAccessMask() != VK_ACCESS_TRANSFER_WRITE_BIT)
      dst_buffer.Transition(VK_ACCESS_TRANSFER_WRITE_BIT);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();
    if (specifics.CopyData.size())
    {
      for (const VkBufferImageCopy& copyData : specifics.CopyData)
      {
        CORE_ASSERT(copyData.bufferOffset + copyData.bufferRowLength * copyData.bufferImageHeight <= dst_buffer.GetSize(), "[VULKAN/SYSTEM] Copying off-range data to the destination buffer");
        CORE_ASSERT(copyData.imageOffset.x + copyData.imageExtent.width <= m_Extent.width && copyData.imageOffset.y + copyData.imageExtent.height <= m_Extent.height && copyData.imageOffset.z + copyData.imageExtent.depth <= m_Extent.depth, "[VULKAN/SYSTEM] Copying to an area that is out of range of the source image");

        CORE_ASSERT(copyData.imageSubresource.baseArrayLayer + copyData.imageSubresource.layerCount <= m_LayerCount, "[VULKAN/SYSTEM] Copying more layers than available in the source image");
        CORE_ASSERT(copyData.imageSubresource.mipLevel <= m_MipmapLevels, "[VULKAN/SYSTEM] Specified mip level exceeds the available mip levels in the source image");
      }

      vkCmdCopyImageToBuffer(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_buffer.Get(), static_cast<uint32_t>(specifics.CopyData.size()), specifics.CopyData.data());
    }
    else
    {
      VkBufferImageCopy region = {};
      region.bufferOffset = 0;
      region.bufferRowLength = 0;
      region.bufferImageHeight = 0;
      region.imageSubresource.aspectMask = m_Aspect;
      region.imageSubresource.baseArrayLayer = 0;
      region.imageSubresource.layerCount = m_LayerCount;
      region.imageSubresource.mipLevel = 0;
      region.imageOffset = { 0, 0, 0 };
      region.imageExtent = m_Extent;

      vkCmdCopyImageToBuffer(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_buffer.Get(), 1, &region);
    }
    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    CORE_INFO("[SYSTEM] Image '{}' copied to buffer '{}'", (int)m_Image, (int)dst_buffer.Get());
  }

  void Image::CopyToBuffer(Buffer& dst_buffer, const VkBufferImageCopy& copy_data)
  {
    ImageToBufferCopySpecifics specifics = {};
    specifics.CopyData = { copy_data };
    Image::CopyToBuffer(dst_buffer, specifics);
  }

  Image Image::Create(VkImageType type, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkImageCreateFlags flags, VkImageTiling tiling, VkSampleCountFlagBits sample_count, uint32_t mipmap_levels, uint32_t layer_count)
  {
    switch (type)
    {
    case VK_IMAGE_TYPE_1D:
    {
      CORE_ASSERT(extent.height == 1 && extent.depth == 1, "[VULKAN/SYSTEM] Invalid height/depth specified for a 1D Image");
      break;
    }
    case VK_IMAGE_TYPE_2D:
    {
      CORE_ASSERT(extent.depth == 1, "[VULKAN/SYSTEM] Invalid depth specified for a 2D Image");
      if (flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
        CORE_ASSERT(!(layer_count % 6), "[VULKAN/SYSTEM] If the image is a cubemap, the layer count should be a multiple of 6");
      break;
    }
    case VK_IMAGE_TYPE_3D:
    {
      CORE_ASSERT(layer_count == 1, "[VULKAN/SYSTEM] Invalid layer count specified for a 3D Image");
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

    image.m_Format = format;
    image.m_Usage = usage;
    image.m_Extent = extent;
    image.m_MipmapLevels = mipmap_levels;
    image.m_LayerCount = layer_count;
    image.m_Type = type;

    image.m_Aspect = Utils::VkFormatToVkAspectMask(format);

    image.m_Size = Utils::VkFormatToByteSize(format) * extent.width * extent.height * extent.depth * layer_count;

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.flags = flags;
    imageCreateInfo.imageType = type;
    imageCreateInfo.extent = extent;
    imageCreateInfo.mipLevels = mipmap_levels;
    imageCreateInfo.arrayLayers = layer_count;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.samples = sample_count; // TO RETHINK
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(image.r_Device->GetLogical(), &imageCreateInfo, VK_NULL_HANDLE, &image.m_Image);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image. {}", Utils::VkResultToString(result));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(image.r_Device->GetLogical(), image.m_Image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo = {};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = image.r_Device->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memory_properties);

    result = vkAllocateMemory(image.r_Device->GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &image.m_Memory);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory for the image. {}", Utils::VkResultToString(result));

    result = vkBindImageMemory(image.r_Device->GetLogical(), image.m_Image, image.m_Memory, 0);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the image memory. {}", Utils::VkResultToString(result));

    return image;
  }

  Image Image::Create(const ImageCreateInfo& info)
  {
    return Image::Create(info.Type, info.Extent, info.Format, info.Usage, info.MemoryProperties, info.Flags, info.Tiling, info.SampleCount, info.MipmapLevels, info.LayerCount);
  }

}
