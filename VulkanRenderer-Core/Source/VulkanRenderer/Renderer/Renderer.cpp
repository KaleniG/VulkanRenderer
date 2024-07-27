#include <vkrenpch.h>

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  struct alignas(16) UniformBufferObject
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  void Renderer::Init(const RendererConfig& config)
  {
    CORE_ASSERT(!Renderer::Get().m_Initialized, "[SYSTEM] Renderer is already initialized");
    Renderer::Get().m_Initialized = true;
    Renderer::Get().m_Config = config;

    Renderer::Get().m_Device = CreateRef<Device>(Renderer::GetConfig().Device);
    Renderer::Get().m_Swapchain = CreateRef<Swapchain>();

    Renderer::Get().m_Texture = CreateRef<Texture>("Assets/Textures/texture.png");

    Renderer::Get().m_UniformBuffers.resize(Renderer::GetDevice().GetConfig().MaxFramesInFlight);
    for (Ref<UniformBuffer>& buffer : Renderer::Get().m_UniformBuffers)
      buffer = CreateRef<UniformBuffer>(sizeof(UniformBufferObject));

    DescriptorSetConfig descriptorConfig;
    descriptorConfig.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, Renderer::Get().m_UniformBuffers);
    descriptorConfig.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, Renderer::Get().m_Texture);

    Renderer::Get().m_Shader = CreateRef<Shader>("Assets/Shaders/Shader.vert.spv", "Assets/Shaders/Shader.frag.spv", descriptorConfig);
    Renderer::Get().m_GraphicsPipeline = CreateRef<GraphicsPipeline>(Renderer::Get().m_Shader);
  }

  void Renderer::Shutdown()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    Renderer::Get().m_Initialized = false;
  }

  Device& Renderer::GetDevice()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return *Renderer::Get().m_Device.get();
  }

  Ref<Device>& Renderer::GetDeviceRef()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return Renderer::Get().m_Device;
  }

  const RendererConfig& Renderer::GetConfig()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return Renderer::Get().m_Config;
  }

}