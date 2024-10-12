#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/EngineComponents/Fence.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  Fence::~Fence()
  {
    vkDestroyFence(Renderer::GetDevice().GetLogical(), m_Fence, VK_NULL_HANDLE);
  }

  void Fence::Wait(uint64_t timeout)
  {
    vkWaitForFences(Renderer::GetDevice().GetLogical(), 1, &m_Fence, VK_TRUE, timeout);
  }

  void Fence::Reset()
  {
    vkResetFences(Renderer::GetDevice().GetLogical(), 1, &m_Fence);
  }

  void Fence::Wait(const std::vector<Ref<Fence>>& fences, bool wait_all, uint64_t timeout)
  {
    std::vector<VkFence> vkFences(fences.size());
    for (const Ref<Fence>& fence : fences)
      vkFences.emplace_back(fence->Get());

    vkWaitForFences(Renderer::GetDevice().GetLogical(), vkFences.size(), vkFences.data(), wait_all ? VK_TRUE : VK_FALSE, timeout);
  }

  void Fence::Reset(const std::vector<Ref<Fence>>& fences)
  {
    std::vector<VkFence> vkFences(fences.size());
    for (const Ref<Fence>& fence : fences)
      vkFences.emplace_back(fence->Get());

    vkResetFences(Renderer::GetDevice().GetLogical(), vkFences.size(), vkFences.data());
  }

  Ref<Fence> Fence::Create(VkFenceCreateFlags flags)
  {
    Ref<Fence> fence = CreateRef<Fence>();

    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateFence(Renderer::GetDevice().GetLogical(), &createInfo, VK_NULL_HANDLE, &fence->m_Fence);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a fence. Error: {}", Utils::VkResultToString(result));

    return fence;
  }
}