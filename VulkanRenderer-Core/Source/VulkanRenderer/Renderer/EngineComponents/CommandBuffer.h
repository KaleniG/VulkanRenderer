#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/EngineComponents/CommandPool.h"
#include "VulkanRenderer/Renderer/EngineComponents/Semaphore.h"
#include "VulkanRenderer/Renderer/EngineComponents/Fence.h"

namespace vkren
{

  class CommandBuffer
  {
  public:
    ~CommandBuffer();

    const VkCommandBuffer& Get() const;
    const Ref<CommandPool>& GetPool() const;

    void Deallocate();

    void Submit(const Ref<Semaphore>& wait_semaphore, VkPipelineStageFlags wait_stage, const Ref<Semaphore>& signal_semaphore, const Ref<Fence>& signal_fence);

    static Ref<CommandBuffer> Allocate(const Ref<CommandPool>& pool, VkCommandBufferLevel level);

  private:
    Ref<CommandPool> r_Pool = nullptr;
    VkCommandBuffer m_Buffer = VK_NULL_HANDLE;
  };

}