#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  Renderer::Renderer(Window& window, const RendererConfig& config)
    : r_Window(window),
      m_Device(r_Window, config.Device),
      m_Swapchain(m_Device, r_Window)
  {}

  void Renderer::Init()
  {
    DescriptorSetConfig descriptorConfig;
    descriptorConfig.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    descriptorConfig.AddBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

    m_Shader = CreateRef<Shader>(m_Device, "Assets/Shaders/Shader.vert.spv", "Assets/Shaders/Shader.frag.spv", descriptorConfig);

    m_GraphicsPipeline = CreateRef<GraphicsPipeline>(m_Device, m_Shader);
  }

  void Renderer::Shutdown()
  {
    m_GraphicsPipeline->Destroy();
    m_Shader->Destroy();
    m_Swapchain.Destroy();
  }

}