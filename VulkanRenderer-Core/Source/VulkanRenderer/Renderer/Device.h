#pragma once

#include <imgui.h>
#include <vulkan/vulkan.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "VulkanRenderer/Core/Window.h"

namespace vkren
{
  class Swapchain;
  class GraphicsPipeline;
  class MUniformBuffer;
  class VertexBuffer;
  class IndexBuffer;

  struct DeviceConfig
  {
    uint32_t MaxFramesInFlight;
  };

  class Device
  {
  public:
    Device(const DeviceConfig& config);
    ~Device();

    void WaitIdle();

    const DeviceConfig& GetConfig() const { return m_DeviceConfig; }

    const VkInstance& GetVulkanInstance() const { return m_VulkanInstance; }
    const VkPhysicalDevice& GetPhysical() const { return m_PhysicalDevice; }
    const VkDevice& GetLogical() const { return m_LogicalDevice; }
    const VkSurfaceKHR& GetSurface() const { return m_Surface; }
    const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }

    uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
    const VkQueue& GetGraphicsQueue() const { return m_GraphicsQueue; }
    uint32_t GetPresentQueueFamilyIndex() const { return m_PresentQueueFamilyIndex; }
    const VkQueue& GetPresentQueue() const { return m_PresentQueue; }

    uint32_t GetMinSwapchainImageCount();
    const VkExtent2D& GetSurfaceExtent() const;
    const VkFormat& GetDepthAttachmentFormat() const { return m_DepthAttachmentFormat; }
    const VkSampleCountFlagBits& GetMaxSampleCount() const { return m_MaxSampleCount; }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory);
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

    uint32_t FindMemoryTypeIndex(uint32_t type_filter, VkMemoryPropertyFlags properties);
    VkFormatProperties GetFormatProperties(const VkFormat& format);

    void CmdTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
    void CmdCopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void CmdCopyBufferToBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

    VkCommandBuffer GetSingleTimeCommandBuffer();
    void SubmitSingleTimeCommandBuffer(VkCommandBuffer command_buffer);

  private:
    void CreateVulkanInstance();
    void CreateDebugMessenger();
    void CreateSurface();
    void ChoosePhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandSystem();

  private:
    DeviceConfig m_DeviceConfig;

    VkInstance m_VulkanInstance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkSurfaceKHR m_Surface;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_LogicalDevice;

    uint32_t m_GraphicsQueueFamilyIndex;
    VkQueue m_GraphicsQueue;
    uint32_t m_PresentQueueFamilyIndex;
    VkQueue m_PresentQueue;

    VkFormat m_DepthAttachmentFormat;

    VkCommandPool m_CommandPool;

    VkPhysicalDeviceMemoryProperties m_PhysicalDeviceMemoryProperties;
    VkPhysicalDeviceProperties m_PhysicalDeviceProperties;

    VkSampleCountFlagBits m_MaxSampleCount;
  };

}