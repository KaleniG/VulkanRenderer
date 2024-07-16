#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  Renderer::Renderer(Window& window) 
    : r_Window(window),
      m_Device(r_Window),
      m_Swapchain(m_Device, r_Window)
  {}

  void Renderer::Init()
  {
    DescriptorSetBindings shaderBindings;
    shaderBindings.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    shaderBindings.AddBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

    m_Shader = CreateRef<Shader>(m_Device, "Assets/Shaders/Shader.vert.spv", "Assets/Shaders/Shader.frag.spv", shaderBindings);

    m_GraphicsPipeline = CreateRef<GraphicsPipeline>(m_Device, m_Shader);
  }

  void Renderer::Shutdown()
  {
    m_GraphicsPipeline->Destroy();
    m_Shader->Destroy();
    m_Swapchain.Destroy();
  }

}