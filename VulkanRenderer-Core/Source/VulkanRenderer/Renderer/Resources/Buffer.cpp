#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/Buffer.h"
#include "VulkanRenderer/Renderer/Utils/Functions.h"
#include "VulkanRenderer/Renderer/Utils/Debug.h"

namespace vkren
{

  Buffer::~Buffer()
  {
    vkDestroyBuffer(r_Device->GetLogical(), m_Buffer, VK_NULL_HANDLE);
    vkFreeMemory(r_Device->GetLogical(), m_Memory, VK_NULL_HANDLE);
  }

  void Buffer::Transition(const VkAccessFlags& new_access, const BufferTransitionSpecifics& specifics) // TEMPORARY BECAUSE I WANT TO IMPLEMENT A GLOBAL MEMORY AND COMMAND BUFFERS MANAGEMENT SYSTEM
  {
    if (m_CurrentAccessMask == new_access)
    {
      CORE_WARN("[VULKAN/SYSTEM] Transitioning the buffer to the same access it is at the moment");
      return;
    }

    Debug::AccessMaskToBufferUsageCheck(m_Usage, new_access);

    VkBufferMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.buffer = m_Buffer;
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;
    barrier.srcAccessMask = m_CurrentAccessMask;
    barrier.dstAccessMask = new_access;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    VkPipelineStageFlags dstStages = Utils::VkAccessMaskToVkPipelineStagesMask(new_access, specifics.PipelineStagesMask);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();
    vkCmdPipelineBarrier(commandBuffer, m_CurrentPipelineStageMask, dstStages, 0, 0, VK_NULL_HANDLE, 1, &barrier, 0, VK_NULL_HANDLE);
    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    m_CurrentAccessMask = new_access;
    m_CurrentPipelineStageMask = dstStages;
  }

  void Buffer::CopyToBuffer(Buffer& dst_buffer, const std::vector<VkBufferCopy>& copy_regions)
  {
    CORE_ASSERT(m_Usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "[VULKAN/SYSTEM] The source buffer cannot be used as a data transfer source, 'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its buffer creation");
    CORE_ASSERT(dst_buffer.m_Usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT, "[VULKAN/SYSTEM] The destination buffer cannot be used as a data transfer destination, 'VK_BUFFER_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during the buffer creation");

    if (m_Used && !(m_CurrentAccessMask & VK_ACCESS_TRANSFER_READ_BIT) && m_CurrentAccessMask != VK_ACCESS_NONE)
      Buffer::Transition(VK_ACCESS_TRANSFER_READ_BIT);

    if (dst_buffer.m_Buffer == m_Buffer)
      CORE_WARN("[SYSTEM] Copying a buffer to itself");
    else if (dst_buffer.m_Used && !(dst_buffer.m_CurrentAccessMask & VK_ACCESS_TRANSFER_WRITE_BIT) && dst_buffer.m_CurrentAccessMask != VK_ACCESS_NONE)
      dst_buffer.Transition(VK_ACCESS_TRANSFER_WRITE_BIT);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();
    if (copy_regions.size())
    {
      for (const VkBufferCopy& copyRegion : copy_regions)
      {
        CORE_ASSERT((copyRegion.size + copyRegion.srcOffset) <= m_Size, "[VULKAN/SYSTEM] Copying off-range data from the source buffer");
        CORE_ASSERT((copyRegion.size + copyRegion.dstOffset) <= dst_buffer.m_Size, "[VULKAN/SYSTEM] Copying to an area that is out of range of the destination buffer");
      }

      vkCmdCopyBuffer(commandBuffer, m_Buffer, dst_buffer.m_Buffer, static_cast<uint32_t>(copy_regions.size()), copy_regions.data());
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
    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    m_Used = true;
    m_CurrentAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    dst_buffer.m_Used = true;
    dst_buffer.m_CurrentAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    dst_buffer.m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }

  void Buffer::CopyToImage(Image& dst_image, const std::vector<VkBufferImageCopy>& copy_regions)
  {
    CORE_ASSERT(m_Usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "[VULKAN/SYSTEM] The source buffer cannot be used as a data transfer source, 'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its buffer creation");
    CORE_ASSERT(dst_image.GetUsage() & VK_IMAGE_USAGE_TRANSFER_DST_BIT, "[VULKAN/SYSTEM] The destination image cannot be used as a data transfer destination, 'VK_IMAGE_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during the image creation");

    if (m_Used && !(m_CurrentAccessMask & VK_ACCESS_TRANSFER_READ_BIT))
      Buffer::Transition(VK_ACCESS_TRANSFER_READ_BIT);

    if (dst_image.GetLayout() != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
      dst_image.Transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();
    if (copy_regions.size())
    {
      for (const VkBufferImageCopy& copyRegion : copy_regions)
      {
        CORE_ASSERT((copyRegion.bufferOffset + copyRegion.bufferRowLength * copyRegion.bufferImageHeight) <= m_Size, "[VULKAN/SYSTEM] Copying off-range data from the source buffer");
        CORE_ASSERT(copyRegion.imageOffset.x + copyRegion.imageExtent.width <= dst_image.GetExtent().width && copyRegion.imageOffset.y + copyRegion.imageExtent.height <= dst_image.GetExtent().height && copyRegion.imageOffset.z + copyRegion.imageExtent.depth <= dst_image.GetExtent().depth, "[VULKAN/SYSTEM] Copying to an area that is out of range of the destination image");

        CORE_ASSERT(copyRegion.imageSubresource.baseArrayLayer + copyRegion.imageSubresource.layerCount <= dst_image.GetLayerCount(), "[VULKAN/SYSTEM] Copying more layers than available in the destination image");
        CORE_ASSERT(copyRegion.imageSubresource.mipLevel <= dst_image.GetMipmapLevels(), "[VULKAN/SYSTEM] Specified mip level exceeds the available mip levels in the destination image");
      }

      vkCmdCopyBufferToImage(commandBuffer, m_Buffer, dst_image.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(copy_regions.size()), copy_regions.data());
    }
    else
    {
      CORE_ASSERT(m_Size <= dst_image.GetSize(), "[VULKAN/SYSTEM] The source buffer size is greater than the destination image's size during whole buffer copy");
      if (m_Size < dst_image.GetSize())
        CORE_WARN("[VULKAN/SYSTEM] Executing complete copy of a source buffer (size:{0}) to a destination image with bigger size (size:{1})", m_Size, dst_image.GetSize());

      VkBufferImageCopy region = {};
      region.imageExtent = dst_image.GetExtent();
      region.imageSubresource.baseArrayLayer = 0;
      region.imageSubresource.layerCount = dst_image.GetLayerCount();
      region.imageSubresource.mipLevel = dst_image.GetMipmapLevels();
      region.imageSubresource.aspectMask = dst_image.GetAspect();

      vkCmdCopyBufferToImage(commandBuffer, m_Buffer, dst_image.Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    m_Used = true;
    m_CurrentAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    m_CurrentPipelineStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }

  void Buffer::CopyToBuffer(Buffer& dst_buffer, const VkBufferCopy& copy_region)
  {
    std::vector<VkBufferCopy> region = { copy_region };
    Buffer::CopyToBuffer(dst_buffer, region);
  }

  void Buffer::CopyToImage(Image& dst_image, const VkBufferImageCopy& copy_region)
  {
    std::vector<VkBufferImageCopy> region = { copy_region };
    Buffer::CopyToImage(dst_image, region);
  }

  Buffer Buffer::Create(VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkDeviceSize size)
  {
    Buffer buffer;

    buffer.r_Device = Renderer::GetDeviceRef();

    buffer.m_Size = size;
    buffer.m_Usage = usage;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = buffer.m_Size;
    bufferCreateInfo.usage = buffer.m_Usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // MAYBE NEED TO TWEAK TO BE COMPATIBLE WITH DEVICES THAT DON'T HAVE TRANSFER AND GRAPHICS IN THE SAME FAMILY IDK

    VkResult result = vkCreateBuffer(buffer.r_Device->GetLogical(), &bufferCreateInfo, VK_NULL_HANDLE, &buffer.m_Buffer);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the buffer. {}", Utils::VkResultToString(result));

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(buffer.r_Device->GetLogical(), buffer.m_Buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo = {};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = buffer.r_Device->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memory_properties);

    result = vkAllocateMemory(buffer.r_Device->GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &buffer.m_Memory);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory for the buffer. {}", Utils::VkResultToString(result));

    result = vkBindBufferMemory(buffer.r_Device->GetLogical(), buffer.m_Buffer, buffer.m_Memory, 0);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the buffer memory. {}", Utils::VkResultToString(result));

    return buffer;
  }

}
