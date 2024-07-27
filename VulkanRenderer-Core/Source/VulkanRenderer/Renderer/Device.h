#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "VulkanRenderer/Core/Window.h"

namespace vkren
{

  struct DeviceConfig
  {
    uint32_t MaxFramesInFlight;
  };

  class Device
  {
  public:
    Device(const DeviceConfig& config);
    ~Device();

    const DeviceConfig& GetConfig() const { return m_DeviceConfig; }

    const VkPhysicalDevice& GetPhysical() const { return m_PhysicalDevice; }
    const VkDevice& GetLogical() const { return m_LogicalDevice; }
    const VkSurfaceKHR& GetSurface() const { return m_Surface; }
    const VkRenderPass& GetRenderPass() const { return m_RenderPass; }

    uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
    const VkQueue& GetGraphicsQueue() const { return m_GraphicsQueue; }
    uint32_t GetPresentQueueFamilyIndex() const { return m_PresentQueueFamilyIndex; }
    const VkQueue& GetPresentQueue() const { return m_PresentQueue; }

    const VkExtent2D& GetSurfaceExtent() const;
    const VkFormat& GetDepthAttachmentFormat() const { return m_DepthAttachmentFormat; }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory);
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

    uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

    void CmdTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
    void CmdCopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

  private:
    void CreateVulkanInstance();
    void CreateDebugMessenger();
    void CreateSurface();
    void ChoosePhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandSystem();
    void CreateRenderPass();
    void CreateSyncObjects();

    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer command_buffer);

  private:
    DeviceConfig m_DeviceConfig;

    VkInstance m_VulkanInstance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkSurfaceKHR m_Surface;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_LogicalDevice;
    VkRenderPass m_RenderPass;

    uint32_t m_GraphicsQueueFamilyIndex;
    VkQueue m_GraphicsQueue;
    uint32_t m_PresentQueueFamilyIndex;
    VkQueue m_PresentQueue;

    VkFormat m_DepthAttachmentFormat;

    VkCommandPool m_CommandPool;
    std::vector <VkCommandBuffer> m_CommandBuffers;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;
  };

}