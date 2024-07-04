#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Window.h"

namespace vkren
{

  class Renderer
  {
  public:
    Renderer() = default;

    void Init();
    void Shutdown();

  private:
    void CreateVulkanInstance();
    void CreateDebugMessenger();

  private:
    void DestroyVulkanInstance();
    void DestroyDebugMessenger();

  private:
    Window* m_WindowReference;
    VkInstance m_VulkanInstance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
  };

}