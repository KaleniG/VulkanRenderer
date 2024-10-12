#pragma once

#include <vulkan/vulkan.h>

namespace vkren
{

  class CommandPool
  {
  public:
    ~CommandPool();

    const VkCommandPool& Get() const { return m_Pool; }

    static Ref<CommandPool> Create(uint32_t queue_family_index, VkCommandPoolCreateFlags flags);

  private:
    VkCommandPool m_Pool = VK_NULL_HANDLE;
  };

}