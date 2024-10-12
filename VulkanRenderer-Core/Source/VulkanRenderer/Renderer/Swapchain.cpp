#include <vkrenpch.h>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Swapchain.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  Swapchain::~Swapchain()
  {
    Swapchain::Clean();
  }

  uint32_t Swapchain::AcquireNextImage(const Ref<Semaphore>& semaphore, const Ref<Fence>& signal_fence, uint64_t timeout)
  {
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(Renderer::GetDevice().GetLogical(), m_Swapchain, timeout, semaphore->Get(), signal_fence ? signal_fence->Get() : VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      Swapchain::Recreate();
      return -1;
    }
    else
      CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "[VULKAN] Failed to acquire the swapchain image. {}", Utils::VkResultToString(result));

    return imageIndex;
  }

  void Swapchain::Present(uint32_t image_index, const Ref<Semaphore>& wait_semaphore)
  {
    VkSemaphore waitSemaphores[] = { wait_semaphore->Get() };
    VkSwapchainKHR swapchains[] = { m_Swapchain };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &image_index;
    presentInfo.pResults = VK_NULL_HANDLE;

    VkResult result = vkQueuePresentKHR(Renderer::GetDevice().GetPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
      Swapchain::Recreate();
    else
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to present the swapchain image. {}", Utils::VkResultToString(result));
  }

  void Swapchain::Recreate()
  {
    Window& window = Application::GetWindow();

    int32_t width = 0, height = 0;
    glfwGetFramebufferSize(window.GetNative(), &width, &height);
    while (width == 0 || height == 0)
    {
      glfwGetFramebufferSize(window.GetNative(), &width, &height);
      glfwWaitEvents();
    }

    Renderer::GetDevice().WaitIdle();

    Swapchain::Clean();
    Swapchain::Create();
  }

  Ref<Swapchain> Swapchain::Create(const Ref<RenderPass>& renderpass)
  {
    Ref<Swapchain> swapchain = CreateRef<Swapchain>();
    swapchain->r_RenderPass = renderpass;
    swapchain->Create();
    return swapchain;
  }

  void Swapchain::Create()
  {
    Renderer::GetDevice().GetPhysical();
    Renderer::GetDevice().GetSurface();

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Renderer::GetDevice().GetPhysical(), Renderer::GetDevice().GetSurface(), &surfaceCapabilities);

    // SWAPCHAIN IMAGE COUNT
    m_SwapchainImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && m_SwapchainImageCount > surfaceCapabilities.maxImageCount)
      m_SwapchainImageCount = surfaceCapabilities.maxImageCount;

    // SURFACE IMAGE FORMAT
    uint32_t surfaceImageFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Renderer::GetDevice().GetPhysical(), Renderer::GetDevice().GetSurface(), &surfaceImageFormatCount, VK_NULL_HANDLE);
    std::vector<VkSurfaceFormatKHR> surfaceImageFormats(surfaceImageFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(Renderer::GetDevice().GetPhysical(), Renderer::GetDevice().GetSurface(), &surfaceImageFormatCount, surfaceImageFormats.data());
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
    vkGetPhysicalDeviceSurfacePresentModesKHR(Renderer::GetDevice().GetPhysical(), Renderer::GetDevice().GetSurface(), &surfacePresentModesCount, VK_NULL_HANDLE);
    std::vector<VkPresentModeKHR> surfacePresentModes(surfacePresentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(Renderer::GetDevice().GetPhysical(), Renderer::GetDevice().GetSurface(), &surfacePresentModesCount, surfacePresentModes.data());

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
    VkExtent2D surfaceExtent = Renderer::GetDevice().GetSurfaceExtent();

    // SWAPCHAIN CREATION
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = Renderer::GetDevice().GetSurface();
    swapchainCreateInfo.minImageCount = m_SwapchainImageCount;
    swapchainCreateInfo.imageFormat = m_SwapchainFormat.format;
    swapchainCreateInfo.imageColorSpace = m_SwapchainFormat.colorSpace;
    swapchainCreateInfo.imageExtent = surfaceExtent;
    swapchainCreateInfo.imageArrayLayers = 1; // 1 for normal rendering, more for stereoscopic/layered rendering
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = m_SwapchainPresentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (Renderer::GetDevice().GetGraphicsQueueFamilyIndex() != Renderer::GetDevice().GetPresentQueueFamilyIndex())
    {
      std::array<uint32_t, 2> queueFamilyIndices = { Renderer::GetDevice().GetGraphicsQueueFamilyIndex(), Renderer::GetDevice().GetPresentQueueFamilyIndex() };
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

    VkResult result = vkCreateSwapchainKHR(Renderer::GetDevice().GetLogical(), &swapchainCreateInfo, VK_NULL_HANDLE, &m_Swapchain);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the swapchain. Error: {}", Utils::VkResultToString(result));

    // EXTENT
    m_Extent = surfaceExtent;

    // SWAPCHAIN IMAGES AND IMAGE VIEWS
    vkGetSwapchainImagesKHR(Renderer::GetDevice().GetLogical(), m_Swapchain, &m_SwapchainImageCount, VK_NULL_HANDLE);
    std::vector<VkImage> swapchainImages(m_SwapchainImageCount);
    vkGetSwapchainImagesKHR(Renderer::GetDevice().GetLogical(), m_Swapchain, &m_SwapchainImageCount, swapchainImages.data());

    m_SwapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++)
      m_SwapchainImageViews[i] = Renderer::GetDevice().CreateImageView(swapchainImages[i], m_SwapchainFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

    // DEPTH IMAGE AND IMAGE VIEW
    Renderer::GetDevice().CreateImage(m_Extent.width, m_Extent.height, Renderer::GetDevice().GetDepthAttachmentFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
    m_DepthImageView = Renderer::GetDevice().CreateImageView(m_DepthImage, Renderer::GetDevice().GetDepthAttachmentFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);

    Renderer::GetDevice().CmdTransitionImageLayout(m_DepthImage, Renderer::GetDevice().GetDepthAttachmentFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // FRAMEBUFFERS
    m_Framebuffers.resize(swapchainImages.size());

    for (size_t i = 0; i < m_SwapchainImageViews.size(); i++) {
      std::array<VkImageView, 2> attachments = { m_SwapchainImageViews[i], m_DepthImageView };

      VkFramebufferCreateInfo framebufferCreateInfo{};
      framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferCreateInfo.renderPass = r_RenderPass->Get();
      framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      framebufferCreateInfo.pAttachments = attachments.data();
      framebufferCreateInfo.width = m_Extent.width;
      framebufferCreateInfo.height = m_Extent.height;
      framebufferCreateInfo.layers = 1;

      VkResult result = vkCreateFramebuffer(Renderer::GetDevice().GetLogical(), &framebufferCreateInfo, VK_NULL_HANDLE, &m_Framebuffers[i]);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a swapchain framebuffer. Error: {}", Utils::VkResultToString(result));
    }
  }

  void Swapchain::Clean()
  {
    vkDestroyImageView(Renderer::GetDevice().GetLogical(), m_DepthImageView, VK_NULL_HANDLE);
    vkDestroyImage(Renderer::GetDevice().GetLogical(), m_DepthImage, VK_NULL_HANDLE);
    vkFreeMemory(Renderer::GetDevice().GetLogical(), m_DepthImageMemory, VK_NULL_HANDLE);

    for (VkFramebuffer framebuffer : m_Framebuffers)
      vkDestroyFramebuffer(Renderer::GetDevice().GetLogical(), framebuffer, VK_NULL_HANDLE);

    for (VkImageView imageView : m_SwapchainImageViews)
      vkDestroyImageView(Renderer::GetDevice().GetLogical(), imageView, VK_NULL_HANDLE);

    vkDestroySwapchainKHR(Renderer::GetDevice().GetLogical(), m_Swapchain, VK_NULL_HANDLE);
  }

}