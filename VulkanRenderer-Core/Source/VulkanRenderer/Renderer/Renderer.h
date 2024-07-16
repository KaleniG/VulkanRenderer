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

  class Renderer
  {
  public:
    Renderer(Window& window);

    void Init();
    void Shutdown();

  private:
    Window& r_Window;

    Device m_Device;
    Swapchain m_Swapchain;

    Ref<Shader> m_Shader;
    Ref<GraphicsPipeline> m_GraphicsPipeline;
  };

}