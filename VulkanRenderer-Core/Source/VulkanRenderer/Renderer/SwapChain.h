#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace vkren
{

  struct SwapChain
  {
    VkSwapchainKHR Self;
    std::optional<VkSurfaceFormatKHR> ImageFormat;
    VkExtent2D Extent;
    std::vector<VkImageView> ImageViews;
    std::vector<VkFramebuffer> Framebuffers;
  };

}