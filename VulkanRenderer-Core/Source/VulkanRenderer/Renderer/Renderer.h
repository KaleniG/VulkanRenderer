#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Window.h"
#include "VulkanRenderer/Renderer/QueueFamily.h"

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
    void CreateSurface();
    void ChoosePhysicalDevice();
    void CreateLogicalDevice();

  private:
    void DestroyVulkanInstance();
    void DestroyDebugMessenger();
    void DestroySurface();
    void DestroyLogicalDevice();

  private:
    Window* m_WindowReference;

    VkInstance m_VulkanInstance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkSurfaceKHR m_Surface;
    VkPhysicalDevice m_PhysicalDevice;
    QueueInfo m_GraphicsQueueInfo;
    QueueInfo m_PresentQueueInfo;
    VkDevice m_LogicalDevice;
  };

}