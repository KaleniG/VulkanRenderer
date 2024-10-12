#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/EngineComponents/CommandBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  CommandBuffer::~CommandBuffer()
  {
    if (CommandBuffer::Get())
      CommandBuffer::Deallocate();
  }

  const VkCommandBuffer& CommandBuffer::Get() const
  {
    CORE_ASSERT(m_Buffer != VK_NULL_HANDLE, "[SYSTEM] The command buffer is not allocated");
    return m_Buffer;
  }

  const Ref<CommandPool>& CommandBuffer::GetPool() const
  {
    CORE_ASSERT(r_Pool != nullptr, "[SYSTEM] The command pool is not present, the command buffer is not allocated");
    return r_Pool;
  }

  void CommandBuffer::Deallocate()
  {
    vkFreeCommandBuffers(Renderer::GetDevice().GetLogical(), CommandBuffer::GetPool()->Get(), 1, &CommandBuffer::Get());
    m_Buffer = VK_NULL_HANDLE;
  }

  void CommandBuffer::Submit(const Ref<Semaphore>& wait_semaphore, VkPipelineStageFlags wait_stage, const Ref<Semaphore>& signal_semaphore, const Ref<Fence>& signal_fence)
  {
    VkSemaphore waitSemaphores[]      = { wait_semaphore->Get() };
    VkPipelineStageFlags waitStages[] = { wait_stage };
    VkSemaphore signalSemaphores[]    = { signal_semaphore->Get() };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_Buffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkResult result = vkQueueSubmit(Renderer::GetDevice().GetGraphicsQueue(), 1, &submitInfo, signal_fence->Get());
    CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "[VULKAN] Failed to submit the draw command buffer. {}", Utils::VkResultToString(result));
  }

  Ref<CommandBuffer> CommandBuffer::Allocate(const Ref<CommandPool>& pool, VkCommandBufferLevel level)
  {
    Ref<CommandBuffer> buffer = CreateRef<CommandBuffer>();

    buffer->r_Pool = pool;

    VkCommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.commandPool = pool->Get();
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(Renderer::GetDevice().GetLogical(), &commandBufferAllocInfo, &buffer->m_Buffer);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate the command buffer. {}", Utils::VkResultToString(result));

    return buffer;
  }

}