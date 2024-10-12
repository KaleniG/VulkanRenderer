#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/EngineComponents/CommandPool.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{
  CommandPool::~CommandPool()
  {
    vkDestroyCommandPool(Renderer::GetDevice().GetLogical(), m_Pool, VK_NULL_HANDLE);
  }

  Ref<CommandPool> CommandPool::Create(uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
  {
    Ref<CommandPool> pool = CreateRef<CommandPool>();

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.queueFamilyIndex = queue_family_index;

    VkResult result = vkCreateCommandPool(Renderer::GetDevice().GetLogical(), &createInfo, VK_NULL_HANDLE, &pool->m_Pool);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the command pool. {}", Utils::VkResultToString(result));

    return pool;
  }

}