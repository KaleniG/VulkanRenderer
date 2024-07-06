#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Window.h"
#include "VulkanRenderer/Renderer/QueueInfo.h"
#include "VulkanRenderer/Renderer/SwapChain.h"

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
    void CreateSwapChain();

  private:
    void DestroyVulkanInstance();
    void DestroyDebugMessenger();
    void DestroySurface();
    void DestroyLogicalDevice();
    void DestroySwapChain();

  private:
    Window* m_WindowReference;

    VkInstance m_VulkanInstance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkSurfaceKHR m_Surface;
    VkPhysicalDevice m_PhysicalDevice;
    QueueInfo m_GraphicsQueueInfo;
    QueueInfo m_PresentQueueInfo;
    VkDevice m_LogicalDevice;

    SwapChain m_SwapChain;

  };

}