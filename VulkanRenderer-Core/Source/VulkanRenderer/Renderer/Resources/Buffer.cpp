#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/Buffer.h"
#include "VulkanRenderer/Renderer/Resources/Image.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

#define ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, usage, valid_access, valid_usages) if (access & valid_access) CORE_ASSERT(usage & valid_usages, "[VULKAN/SYSTEM] Invalid access mask for this resource's usage");

namespace vkren
{

  Buffer::~Buffer()
  {
    vkDestroyBuffer(Renderer::GetDevice().GetLogical(), m_Buffer, VK_NULL_HANDLE);
  }

  void Buffer::Transition(const VkAccessFlags& new_access, const BufferTransitionSpecifics& specifics)
  {
    if (m_CurrentAccessMask == new_access)
    {
      CORE_WARN("[VULKAN/SYSTEM] Transitioning the buffer to the same access it is at the moment");
      return;
    }

    Buffer::AccessMaskToBufferUsageCheck(new_access);

    VkBufferMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.buffer = m_Buffer;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;
    barrier.srcAccessMask = m_CurrentAccessMask;
    barrier.dstAccessMask = new_access;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    VkPipelineStageFlags dstStages = Resource::AccessMaskToPipelineStages(new_access, specifics.PipelineStagesMask);

    VkCommandBuffer commandBuffer = Renderer::GetDevice().GetSingleTimeCommandBuffer();
    vkCmdPipelineBarrier(commandBuffer, m_CurrentPipelineStageMask, dstStages, 0, 0, VK_NULL_HANDLE, 1, &barrier, 0, VK_NULL_HANDLE);
    Renderer::GetDevice().SubmitSingleTimeCommandBuffer(commandBuffer);

    m_CurrentAccessMask = new_access;
    m_CurrentPipelineStageMask = dstStages;

    CORE_TRACE("[SYSTEM] Buffer '{}' transitioned", (int)m_Buffer);
  }

  void Buffer::CopyToBuffer(Buffer& dst_buffer, const BufferToBufferCopySpecifics& specifics)
  {
    CORE_ASSERT(m_Usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "[VULKAN/SYSTEM] The source buffer cannot be used as a data transfer source, 'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its buffer creation");
    CORE_ASSERT(dst_buffer.m_Usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT, "[VULKAN/SYSTEM] The destination buffer cannot be used as a data transfer destination, 'VK_BUFFER_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during the buffer creation");

    if (m_Used && !(m_CurrentAccessMask & VK_ACCESS_TRANSFER_READ_BIT) && m_CurrentAccessMask != VK_ACCESS_NONE)
      Buffer::Transition(VK_ACCESS_TRANSFER_READ_BIT);

    if (dst_buffer.m_Buffer == m_Buffer)
      CORE_WARN("[SYSTEM] Copying a buffer to itself");
    else if (dst_buffer.m_Used && !(dst_buffer.m_CurrentAccessMask & VK_ACCESS_TRANSFER_WRITE_BIT) && dst_buffer.m_CurrentAccessMask != VK_ACCESS_NONE)
      dst_buffer.Transition(VK_ACCESS_TRANSFER_WRITE_BIT);

    VkCommandBuffer commandBuffer = Renderer::GetDevice().GetSingleTimeCommandBuffer();
    if (specifics.CopyRegions.size())
    {
      for (const VkBufferCopy& copyRegion : specifics.CopyRegions)
      {
        CORE_ASSERT((copyRegion.size + copyRegion.srcOffset) <= m_Size, "[VULKAN/SYSTEM] Copying off-range data from the source buffer");
        CORE_ASSERT((copyRegion.size + copyRegion.dstOffset) <= dst_buffer.m_Size, "[VULKAN/SYSTEM] Copying to an area that is out of range of the destination buffer");
      }

      vkCmdCopyBuffer(commandBuffer, m_Buffer, dst_buffer.m_Buffer, static_cast<uint32_t>(specifics.CopyRegions.size()), specifics.CopyRegions.data());
    }
    else
    {
      CORE_ASSERT(m_Size <= dst_buffer.m_Size, "[VULKAN/SYSTEM] The source buffer size is greater than the destination buffer's size during whole buffer copy");
      if (m_Size < dst_buffer.m_Size)
        CORE_WARN("[VULKAN/SYSTEM] Executing complete copy of a source buffer (size:{0}) to a destination buffer with bigger size (size:{1})", m_Size, dst_buffer.m_Size);

      VkBufferCopy copyRegion = {};
      copyRegion.size = m_Size;

      vkCmdCopyBuffer(commandBuffer, m_Buffer, dst_buffer.m_Buffer, 1, &copyRegion);
    }
    Renderer::GetDevice().SubmitSingleTimeCommandBuffer(commandBuffer);

    m_Used = true;
    m_CurrentAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    dst_buffer.m_Used = true;
    dst_buffer.m_CurrentAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    dst_buffer.m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    CORE_TRACE("[SYSTEM] Buffer '{}' copied to buffer '{}'", (int)m_Buffer, (int)dst_buffer.Get());
  }

  void Buffer::CopyToBuffer(Buffer& dst_buffer, const VkBufferCopy& copy_region)
  {
    BufferToBufferCopySpecifics specifics = {};
    specifics.CopyRegions = { copy_region };

    Buffer::CopyToBuffer(dst_buffer, specifics);
  }

  void Buffer::CopyToImage(Image& dst_image, const BufferToImageCopySpecifics& specifics)
  {
    CORE_ASSERT(m_Usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "[VULKAN/SYSTEM] The source buffer cannot be used as a data transfer source, 'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its buffer creation");
    CORE_ASSERT(dst_image.m_Usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT, "[VULKAN/SYSTEM] The destination image cannot be used as a data transfer destination, 'VK_IMAGE_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during the image creation");

    if (m_Used && !(m_CurrentAccessMask & VK_ACCESS_TRANSFER_READ_BIT))
      Buffer::Transition(VK_ACCESS_TRANSFER_READ_BIT);

    if (dst_image.m_CurrentLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
      dst_image.Transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkCommandBuffer commandBuffer = Renderer::GetDevice().GetSingleTimeCommandBuffer();
    if (specifics.CopyData.size())
    {
      for (const VkBufferImageCopy& copyRegion : specifics.CopyData)
      {
        CORE_ASSERT((copyRegion.bufferOffset + copyRegion.bufferRowLength * copyRegion.bufferImageHeight) <= m_Size, "[VULKAN/SYSTEM] Copying off-range data from the source buffer");
        CORE_ASSERT(copyRegion.imageOffset.x + copyRegion.imageExtent.width <= dst_image.m_Extent.width && copyRegion.imageOffset.y + copyRegion.imageExtent.height <= dst_image.m_Extent.height && copyRegion.imageOffset.z + copyRegion.imageExtent.depth <= dst_image.m_Extent.depth, "[VULKAN/SYSTEM] Copying to an area that is out of range of the destination image");
        CORE_ASSERT(copyRegion.imageSubresource.baseArrayLayer + copyRegion.imageSubresource.layerCount <= dst_image.m_LayerCount, "[VULKAN/SYSTEM] Copying more layers than available in the destination image");
        CORE_ASSERT(copyRegion.imageSubresource.mipLevel <= dst_image.m_MipmapLevels, "[VULKAN/SYSTEM] Specified mip level exceeds the available mip levels in the destination image");
      }

      vkCmdCopyBufferToImage(commandBuffer, m_Buffer, dst_image.m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(specifics.CopyData.size()), specifics.CopyData.data());
    }
    else
    {
      CORE_ASSERT(m_Size <= dst_image.m_Size, "[VULKAN/SYSTEM] The source buffer size is greater than the destination image's size during whole buffer copy");
      if (m_Size < dst_image.m_Size)
        CORE_WARN("[VULKAN/SYSTEM] Executing complete copy of a source buffer (size:{0}) to a destination image with bigger size (size:{1})", m_Size, dst_image.m_Size);

      VkBufferImageCopy region = {};
      region.imageExtent = dst_image.m_Extent;
      region.imageSubresource.baseArrayLayer = 0;
      region.imageSubresource.layerCount = dst_image.m_LayerCount;
      region.imageSubresource.mipLevel = 0;
      region.imageSubresource.aspectMask = dst_image.m_Aspect;

      vkCmdCopyBufferToImage(commandBuffer, m_Buffer, dst_image.m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    Renderer::GetDevice().SubmitSingleTimeCommandBuffer(commandBuffer);

    m_Used = true;
    m_CurrentAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    CORE_TRACE("[SYSTEM] Buffer '{}' copied to image '{}'", (int)m_Buffer, (int)dst_image.m_Image);
  }

  void Buffer::CopyToImage(Image& dst_image, const VkBufferImageCopy& copy_data, bool gen_mipmaps)
  {
    BufferToImageCopySpecifics specifics = {};
    specifics.CopyData = { copy_data };
    specifics.GenerateMipmaps = gen_mipmaps;

    Buffer::CopyToImage(dst_image, specifics);
  }

  void Buffer::AccessMaskToBufferUsageCheck(const VkAccessFlags& access)
  {
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_INDEX_READ_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_UNIFORM_READ_BIT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_SHADER_READ_BIT, VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_SHADER_WRITE_BIT, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_TRANSFER_READ_BIT, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_TRANSFER_WRITE_BIT, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT, VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT, VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT, VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT, VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
    ASSERT_ACCESS_MASK_FOR_RESOURCE_USAGE(access, m_Usage, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR);
  }

  void Buffer::CreateBuffer(const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& memory_properties, const VkDeviceSize& size)
  {
    m_Size = size;
    m_Usage = usage;
    m_MemoryProperties = memory_properties;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = m_Size;
    bufferCreateInfo.usage = m_Usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // MAYBE NEED TO TWEAK TO BE COMPATIBLE WITH DEVICES THAT DON'T HAVE TRANSFER AND GRAPHICS IN THE SAME FAMILY IDK

    VkResult result = vkCreateBuffer(Renderer::GetDevice().GetLogical(), &bufferCreateInfo, VK_NULL_HANDLE, &m_Buffer);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the buffer. {}", Utils::VkResultToString(result));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(Renderer::GetDevice().GetLogical(), m_Buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo = {};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = Renderer::GetDevice().FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memory_properties);

    result = vkAllocateMemory(Renderer::GetDevice().GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &m_Memory);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory for the buffer. {}", Utils::VkResultToString(result));

    result = vkBindBufferMemory(Renderer::GetDevice().GetLogical(), m_Buffer, m_Memory, 0);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the buffer memory. {}", Utils::VkResultToString(result));
  }

}