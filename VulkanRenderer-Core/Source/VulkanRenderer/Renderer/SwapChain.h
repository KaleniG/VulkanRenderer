#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{
  class Swapchain
  {
  public:
    Swapchain();
    ~Swapchain();

    void Clean();

  private:
    Ref<Device> r_Device;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VkImage m_DepthImage;
    VkDeviceMemory m_DepthImageMemory;
    VkImageView m_DepthImageView;
    std::vector<VkImageView> m_SwapchainImageViews;
    std::vector<VkFramebuffer> m_Framebuffers;

    uint32_t m_SwapchainImageCount;
    VkSurfaceFormatKHR m_SwapchainFormat;
    VkPresentModeKHR m_SwapchainPresentMode;
  };

}