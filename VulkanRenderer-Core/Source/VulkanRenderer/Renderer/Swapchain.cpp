#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Swapchain.h"

namespace vkren
{
  Swapchain::Swapchain(Device& device, Window& window)
    : r_Device(device), r_Window(window)
  {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r_Device.GetPhysical(), r_Device.GetSurface(), &surfaceCapabilities);

    // SWAPCHAIN IMAGE COUNT
    m_SwapchainImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && m_SwapchainImageCount > surfaceCapabilities.maxImageCount)
      m_SwapchainImageCount = surfaceCapabilities.maxImageCount;

    // SURFACE IMAGE FORMAT
    uint32_t surfaceImageFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(r_Device.GetPhysical(), r_Device.GetSurface(), &surfaceImageFormatCount, VK_NULL_HANDLE);
    std::vector<VkSurfaceFormatKHR> surfaceImageFormats(surfaceImageFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(r_Device.GetPhysical(), r_Device.GetSurface(), &surfaceImageFormatCount, surfaceImageFormats.data());
    m_SwapchainFormat = surfaceImageFormats[0];
    for (const VkSurfaceFormatKHR& surfaceFormat : surfaceImageFormats)
    {
      if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      {
        m_SwapchainFormat = surfaceFormat;
        break;
      }
    }

    // SWAPCHAIN PRESENT MODE
    uint32_t surfacePresentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(r_Device.GetPhysical(), r_Device.GetSurface(), &surfacePresentModesCount, VK_NULL_HANDLE);
    std::vector<VkPresentModeKHR> surfacePresentModes(surfacePresentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(r_Device.GetPhysical(), r_Device.GetSurface(), &surfacePresentModesCount, surfacePresentModes.data());

    m_SwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const VkPresentModeKHR& surfacePresentMode : surfacePresentModes)
    {
      if (surfacePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      {
        m_SwapchainPresentMode = surfacePresentMode;
        break;
      }
    }

    // SURFACE EXTENT
    VkExtent2D surfaceExtent = r_Device.GetSurfaceExtent();

    // SWAPCHAIN CREATION
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = r_Device.GetSurface();
    swapchainCreateInfo.minImageCount = m_SwapchainImageCount;
    swapchainCreateInfo.imageFormat = m_SwapchainFormat.format;
    swapchainCreateInfo.imageColorSpace = m_SwapchainFormat.colorSpace;
    swapchainCreateInfo.imageExtent = surfaceExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = m_SwapchainPresentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (r_Device.GetGraphicsQueueFamilyIndex() != r_Device.GetPresentQueueFamilyIndex())
    {
      std::array<uint32_t, 2> queueFamilyIndices = { r_Device.GetGraphicsQueueFamilyIndex(),  r_Device.GetPresentQueueFamilyIndex() };
      swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
      swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
      swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      swapchainCreateInfo.queueFamilyIndexCount = 0;
      swapchainCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
    }

    VkResult result = vkCreateSwapchainKHR(r_Device.GetLogical(), &swapchainCreateInfo, VK_NULL_HANDLE, &m_Swapchain);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the swapchain");


    // SWAPCHAIN IMAGES AND IMAGE VIEWS
    vkGetSwapchainImagesKHR(r_Device.GetLogical(), m_Swapchain, &m_SwapchainImageCount, VK_NULL_HANDLE);
    std::vector<VkImage> swapchainImages(m_SwapchainImageCount);
    vkGetSwapchainImagesKHR(r_Device.GetLogical(), m_Swapchain, &m_SwapchainImageCount, swapchainImages.data());

    m_SwapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++)
      m_SwapchainImageViews[i] = r_Device.CreateImageView(swapchainImages[i], m_SwapchainFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

    // DEPTH IMAGE AND IMAGE VIEW
    r_Device.CreateImage(surfaceExtent.width, surfaceExtent.height, r_Device.GetDepthAttachmentFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
    m_DepthImageView = r_Device.CreateImageView(m_DepthImage, r_Device.GetDepthAttachmentFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);

    r_Device.CmdTransitionImageLayout(m_DepthImage, r_Device.GetDepthAttachmentFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // FRAMEBUFFERS
    m_Framebuffers.resize(swapchainImages.size());

    for (size_t i = 0; i < m_SwapchainImageViews.size(); i++) {
      std::array<VkImageView, 2> attachments = { m_SwapchainImageViews[i], m_DepthImageView };

      VkFramebufferCreateInfo framebufferCreateInfo{};
      framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferCreateInfo.renderPass = r_Device.GetRenderPass();
      framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      framebufferCreateInfo.pAttachments = attachments.data();
      framebufferCreateInfo.width = surfaceExtent.width;
      framebufferCreateInfo.height = surfaceExtent.height;
      framebufferCreateInfo.layers = 1;

      VkResult result = vkCreateFramebuffer(r_Device.GetLogical(), &framebufferCreateInfo, VK_NULL_HANDLE, &m_Framebuffers[i]);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a swapchain framebuffer");
    }

  }

  void Swapchain::Clean()
  {
    for (VkFramebuffer framebuffer : m_Framebuffers)
      vkDestroyFramebuffer(r_Device.GetLogical(), framebuffer, VK_NULL_HANDLE);

    for (VkImageView imageView : m_SwapchainImageViews)
      vkDestroyImageView(r_Device.GetLogical(), imageView, VK_NULL_HANDLE);

    vkDestroySwapchainKHR(r_Device.GetLogical(), m_Swapchain, VK_NULL_HANDLE);
  }

  void Swapchain::Destroy()
  {
    vkDestroyImageView(r_Device.GetLogical(), m_DepthImageView, VK_NULL_HANDLE);
    vkDestroyImage(r_Device.GetLogical(), m_DepthImage, VK_NULL_HANDLE);
    vkFreeMemory(r_Device.GetLogical(), m_DepthImageMemory, VK_NULL_HANDLE);

    Swapchain::Clean();
  }

}