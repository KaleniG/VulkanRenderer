#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/EngineComponents/RenderPass.h"
#include "VulkanRenderer/Renderer/EngineComponents/Semaphore.h"
#include "VulkanRenderer/Renderer/EngineComponents/Fence.h"
#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{
  class Swapchain
  {
  public:
    ~Swapchain();

    const VkSwapchainKHR& Get() const { return m_Swapchain; }
    const std::vector<VkFramebuffer>& GetFramebuffers() const { return m_Framebuffers; }
    const VkExtent2D& GetExtent() const { return m_Extent; }
    const uint32_t& GetImageCount() const { return m_SwapchainImageCount; }

    uint32_t AcquireNextImage(const Ref<Semaphore>& semaphore, const Ref<Fence>& signal_fence = nullptr, uint64_t timeout = UINT64_MAX); // Not sure about signal_fence
    void Present(uint32_t image_index, const Ref<Semaphore>& wait_semaphore);

    void Recreate();

    static Ref<Swapchain> Create(const Ref<RenderPass>& renderpass);

  private:
    void Create();
    void Clean();


  private:
    Ref<RenderPass> r_RenderPass;

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VkImage m_DepthImage;
    VkDeviceMemory m_DepthImageMemory;
    VkImageView m_DepthImageView;
    std::vector<VkImageView> m_SwapchainImageViews;
    std::vector<VkFramebuffer> m_Framebuffers;

    uint32_t m_SwapchainImageCount;
    VkSurfaceFormatKHR m_SwapchainFormat;
    VkPresentModeKHR m_SwapchainPresentMode;
    VkExtent2D m_Extent;
  };

}