#pragma once

#include <vulkan/vulkan.h>

namespace vkren
{

  class Semaphore
  {
  public:
    ~Semaphore();

    const VkSemaphore& Get() const { return m_Semaphore; }

    static Ref<Semaphore> Create(VkSemaphoreCreateFlags flags = 0);

  private:
    VkSemaphore m_Semaphore = VK_NULL_HANDLE;
  };

}