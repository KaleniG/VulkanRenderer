#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Window.h"
#include "VulkanRenderer/Renderer/GraphicsPipeline.h"
#include "VulkanRenderer/Renderer/Swapchain.h"
#include "VulkanRenderer/Renderer/Device.h"
#include "VulkanRenderer/Renderer/Shader.h"

namespace vkren
{

  struct RendererConfig
  {
    DeviceConfig Device;
  };

  class Renderer
  {
  public:
    Renderer(const Renderer&) = delete;
    Renderer& operator = (const Renderer&) = delete;

    static void Init(const RendererConfig& config);
    static void Shutdown();

    static Device& GetDevice();
    static Ref<Device>& GetDeviceRef();
    static const RendererConfig& GetConfig();

  private:
    Renderer() = default;
    static Renderer& Get() { static Renderer instance; return instance; }

  private:
    bool m_Initialized = false;
    RendererConfig m_Config;

    Ref<Device> m_Device;
    Ref<Swapchain> m_Swapchain;

    Ref<Shader> m_Shader;
    Ref<GraphicsPipeline> m_GraphicsPipeline;
  };

}