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

  void Buffer::Transition(const VkAccessFlags& new_access, const BufferTransitionSpecifics& specifics) // TEMPORARY BECAUSE I WANT TO IMPLEMENT A GLOBAL MEMORY AND COMMAND BUFFERS MANAGEMENTS SYSTEM
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

    VkPipelineStageFlags dstStages = Utils::AccessMaskToPipelineStagesMask(new_access, specifics.PipelineStagesMask);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();
    vkCmdPipelineBarrier(commandBuffer, m_CurrentPipelineStageMask, dstStages, 0, 0, VK_NULL_HANDLE, 1, &barrier, 0, VK_NULL_HANDLE);
    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    m_CurrentAccessMask = new_access;
    m_CurrentPipelineStageMask = dstStages;
  }

  void Buffer::CopyToBuffer(const Buffer& dst_buffer, const std::vector<VkBufferCopy>& copy_regions)
  {
    CORE_ASSERT(m_Usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "[VULKAN/SYSTEM] The source buffer cannot be used as a data transfer source, 'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' usage flag has not been specified during its buffer creation");
    CORE_ASSERT(dst_buffer.GetUsage() & VK_BUFFER_USAGE_TRANSFER_DST_BIT, "[VULKAN/SYSTEM] The destination buffer cannot be used as a data transfer destination, 'VK_BUFFER_USAGE_TRANSFER_DST_BIT' usage flag has not been specified during the buffer creation");
    CORE_ASSERT(copy_regions.size() != 0 || m_Size <= dst_buffer.GetSize(), "[VULKAN/SYSTEM] The source buffer size is greater then the destination buffer's size during whole buffer copy");
    for (const VkBufferCopy& copyRegion : copy_regions)
    {
      CORE_ASSERT((copyRegion.size + copyRegion.srcOffset) <= m_Size, "[VULKAN/SYSTEM] Copying off-range data from the source buffer");
      CORE_ASSERT((copyRegion.size + copyRegion.dstOffset) <= dst_buffer.GetSize(), "[VULKAN/SYSTEM] Copying to an area that is out of range of the destination buffer");
    }

    if (!copy_regions.size() && m_Size < dst_buffer.GetSize())
      CORE_WARN("[VULKAN/SYSTEM] Executing complete copy of a source buffer (size:{0}) to a destination buffer with bigger size (size:{1})", m_Size, dst_buffer.GetSize());

    if (m_Used && m_CurrentAccessMask & VK_ACCESS_TRANSFER_READ_BIT)
      Buffer::Transition(VK_ACCESS_TRANSFER_READ_BIT);

    VkCommandBuffer commandBuffer = r_Device->GetSingleTimeCommandBuffer();

    if (copy_regions.size())
      vkCmdCopyBuffer(commandBuffer, m_Buffer, dst_buffer.Get(), static_cast<uint32_t>(copy_regions.size()), copy_regions.data());
    else
    {
      VkBufferCopy copyRegion = {};
      copyRegion.size = m_Size;

      vkCmdCopyBuffer(commandBuffer, m_Buffer, dst_buffer.Get(), 1, &copyRegion);
    }

    r_Device->SubmitSingleTimeCommandBuffer(commandBuffer);

    m_Used = true;
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
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // MAYBE NEED TO TWEAK TO BE COMPATIBLE WITH DEVICES THAT DONT HAVE TRANSEFER AND GRAPHICS IN THE SAME FAMILY IDK

    VkResult result = vkCreateBuffer(buffer.r_Device->GetLogical(), &bufferCreateInfo, VK_NULL_HANDLE, &buffer.m_Buffer);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(buffer.r_Device->GetLogical(), buffer.m_Buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo = {};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = buffer.r_Device->FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, memory_properties);

    result = vkAllocateMemory(buffer.r_Device->GetLogical(), &memoryAllocInfo, VK_NULL_HANDLE, &buffer.m_Memory);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory for the buffer");

    result = vkBindBufferMemory(buffer.r_Device->GetLogical(), buffer.m_Buffer, buffer.m_Memory, 0);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the buffer memory");

    return buffer;
  }

}