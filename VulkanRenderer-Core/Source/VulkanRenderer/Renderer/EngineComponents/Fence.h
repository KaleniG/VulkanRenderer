#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"

namespace vkren
{

  class Fence
  {
  public:
    ~Fence();

    const VkFence& Get() const { return m_Fence; }

    void Wait(uint64_t timeout = UINT64_MAX);
    void Reset();

    static void Wait(const std::vector<Ref<Fence>>& fences, bool wait_all, uint64_t timeout = UINT64_MAX);
    static void Reset(const std::vector<Ref<Fence>>& fences);


    static Ref<Fence> Create(VkFenceCreateFlags flags = 0);

  private:
    VkFence m_Fence = VK_NULL_HANDLE;
  };

}