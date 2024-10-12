#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/EngineComponents/Semaphore.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{
  Semaphore::~Semaphore()
  {
    vkDestroySemaphore(Renderer::GetDevice().GetLogical(), m_Semaphore, VK_NULL_HANDLE);
  }

  Ref<Semaphore> Semaphore::Create(VkSemaphoreCreateFlags flags)
  {
    Ref<Semaphore> semaphore = CreateRef<Semaphore>();

    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.flags = flags;

    VkResult result = vkCreateSemaphore(Renderer::GetDevice().GetLogical(), &createInfo, VK_NULL_HANDLE, &semaphore->m_Semaphore);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a fence. Error: {}", Utils::VkResultToString(result));

    return semaphore;
  }

}