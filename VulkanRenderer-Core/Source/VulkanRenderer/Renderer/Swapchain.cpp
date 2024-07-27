#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Swapchain.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{
  Swapchain::Swapchain()
    : r_Device(Renderer::GetDeviceRef())
  {
    Device& device = *r_Device.get();

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.GetPhysical(), device.GetSurface(), &surfaceCapabilities);

    // SWAPCHAIN IMAGE COUNT
    m_SwapchainImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && m_SwapchainImageCount > surfaceCapabilities.maxImageCount)
      m_SwapchainImageCount = surfaceCapabilities.maxImageCount;

    // SURFACE IMAGE FORMAT
    uint32_t surfaceImageFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysical(), device.GetSurface(), &surfaceImageFormatCount, VK_NULL_HANDLE);
    std::vector<VkSurfaceFormatKHR> surfaceImageFormats(surfaceImageFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.GetPhysical(), device.GetSurface(), &surfaceImageFormatCount, surfaceImageFormats.data());
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
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysical(), device.GetSurface(), &surfacePresentModesCount, VK_NULL_HANDLE);
    std::vector<VkPresentModeKHR> surfacePresentModes(surfacePresentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.GetPhysical(), device.GetSurface(), &surfacePresentModesCount, surfacePresentModes.data());

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
    VkExtent2D surfaceExtent = device.GetSurfaceExtent();

    // SWAPCHAIN CREATION
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = device.GetSurface();
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

    if (device.GetGraphicsQueueFamilyIndex() != device.GetPresentQueueFamilyIndex())
    {
      std::array<uint32_t, 2> queueFamilyIndices = { device.GetGraphicsQueueFamilyIndex(),  device.GetPresentQueueFamilyIndex() };
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

    VkResult result = vkCreateSwapchainKHR(device.GetLogical(), &swapchainCreateInfo, VK_NULL_HANDLE, &m_Swapchain);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the swapchain");


    // SWAPCHAIN IMAGES AND IMAGE VIEWS
    vkGetSwapchainImagesKHR(device.GetLogical(), m_Swapchain, &m_SwapchainImageCount, VK_NULL_HANDLE);
    std::vector<VkImage> swapchainImages(m_SwapchainImageCount);
    vkGetSwapchainImagesKHR(device.GetLogical(), m_Swapchain, &m_SwapchainImageCount, swapchainImages.data());

    m_SwapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++)
      m_SwapchainImageViews[i] = vkutils::CreateImageView(swapchainImages[i], m_SwapchainFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

    // DEPTH IMAGE AND IMAGE VIEW
    vkutils::CreateImage(surfaceExtent.width, surfaceExtent.height, device.GetDepthAttachmentFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
    m_DepthImageView = vkutils::CreateImageView(m_DepthImage, device.GetDepthAttachmentFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);

    device.CmdTransitionImageLayout(m_DepthImage, device.GetDepthAttachmentFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // FRAMEBUFFERS
    m_Framebuffers.resize(swapchainImages.size());

    for (size_t i = 0; i < m_SwapchainImageViews.size(); i++) {
      std::array<VkImageView, 2> attachments = { m_SwapchainImageViews[i], m_DepthImageView };

      VkFramebufferCreateInfo framebufferCreateInfo{};
      framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferCreateInfo.renderPass = device.GetRenderPass();
      framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      framebufferCreateInfo.pAttachments = attachments.data();
      framebufferCreateInfo.width = surfaceExtent.width;
      framebufferCreateInfo.height = surfaceExtent.height;
      framebufferCreateInfo.layers = 1;

      VkResult result = vkCreateFramebuffer(device.GetLogical(), &framebufferCreateInfo, VK_NULL_HANDLE, &m_Framebuffers[i]);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a swapchain framebuffer");
    }

  }

  Swapchain::~Swapchain()
  {
    Device& device = *r_Device.get();

    vkDestroyImageView(device.GetLogical(), m_DepthImageView, VK_NULL_HANDLE);
    vkDestroyImage(device.GetLogical(), m_DepthImage, VK_NULL_HANDLE);
    vkFreeMemory(device.GetLogical(), m_DepthImageMemory, VK_NULL_HANDLE);

    Swapchain::Clean();
  }

  void Swapchain::Clean()
  {
    Device& device = *r_Device.get();

    for (VkFramebuffer framebuffer : m_Framebuffers)
      vkDestroyFramebuffer(device.GetLogical(), framebuffer, VK_NULL_HANDLE);

    for (VkImageView imageView : m_SwapchainImageViews)
      vkDestroyImageView(device.GetLogical(), imageView, VK_NULL_HANDLE);

    vkDestroySwapchainKHR(device.GetLogical(), m_Swapchain, VK_NULL_HANDLE);
  }

}