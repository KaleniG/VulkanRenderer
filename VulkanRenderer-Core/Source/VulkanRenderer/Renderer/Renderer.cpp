#include <vkrenpch.h>

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Renderer.h"

#include "VulkanRenderer/Renderer/Resources/Buffer.h"

namespace vkren
{

  void Renderer::Init(const RendererConfig& config)
  {
    CORE_ASSERT(!Renderer::Get().m_Initialized, "[SYSTEM] Renderer is already initialized");
    Renderer::Get().m_Initialized = true;
    Renderer::Get().m_Config = config;

    Renderer::Get().m_Device = CreateRef<Device>(Renderer::GetConfig().Device);
    Renderer::Get().m_Swapchain = CreateRef<Swapchain>();

    Buffer buffer1 = Buffer::Create(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 100);
    Buffer buffer2 = Buffer::Create(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 100);
    Buffer buffer3 = Buffer::Create(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 100);

    buffer1.CopyToBuffer(buffer2);
    buffer1.CopyToBuffer(buffer3);
    buffer2.CopyToBuffer(buffer1);
    buffer3.CopyToBuffer(buffer2);
    buffer2.CopyToBuffer(buffer3);

    std::vector<VkBufferCopy> copy_regions = { { 0, buffer1.GetSize() / 2, buffer1.GetSize() / 2 } };
    buffer1.CopyToBuffer(buffer1, copy_regions);

    // TEMP
    Renderer::Get().m_Texture = CreateRef<Texture>("Assets/Textures/texture.png");

    Renderer::Get().m_UniformBuffers.resize(Renderer::GetDevice().GetConfig().MaxFramesInFlight);
    for (Ref<UniformBuffer>& buffer : Renderer::Get().m_UniformBuffers)
      buffer = CreateRef<UniformBuffer>(sizeof(UniformBufferObject));

    DescriptorSetConfig descriptorConfig;
    descriptorConfig.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, Renderer::Get().m_UniformBuffers);
    descriptorConfig.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, Renderer::Get().m_Texture);

    Renderer::Get().m_Shader = CreateRef<Shader>("Assets/Shaders/Shader.vert.spv", "Assets/Shaders/Shader.frag.spv", descriptorConfig);
    Renderer::Get().m_GraphicsPipeline = CreateRef<GraphicsPipeline>(Renderer::Get().m_Shader);

    Renderer::Get().m_Model = CreateRef<Model>("Assets/Models/model.obj");
    Renderer::Get().m_VertexBuffer = CreateRef<VertexBuffer>(Renderer::Get().m_Model->GetVertices());
    Renderer::Get().m_IndexBuffer = CreateRef<IndexBuffer>(Renderer::Get().m_Model->GetIndices());

    Renderer::Get().m_CurrentFrame = 0;
    // TEMP
  }

  void Renderer::Shutdown()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    Renderer::Get().m_Initialized = false;
  }

  void Renderer::DrawFrame(ImDrawData* imgui_draw_data)
  {
    uint32_t& currentFrame = Renderer::Get().m_CurrentFrame;
    Swapchain& swapchain = *Renderer::Get().m_Swapchain.get();
    GraphicsPipeline& pipeline = *Renderer::Get().m_GraphicsPipeline.get();
    UniformBuffer& uniformBuffer = *Renderer::Get().m_UniformBuffers[currentFrame].get();
    VertexBuffer& vertexBuffer = *Renderer::Get().m_VertexBuffer.get();
    IndexBuffer& indexBuffer = *Renderer::Get().m_IndexBuffer.get();
    Renderer::GetDevice().CmdDrawFrame(currentFrame, swapchain, pipeline, uniformBuffer, vertexBuffer, indexBuffer, imgui_draw_data);

    currentFrame = (currentFrame + 1) % Renderer::Get().m_Config.Device.MaxFramesInFlight;
  }

  void Renderer::OnExit()
  {
    Renderer::GetDevice().WaitIdle();
  }

  void Renderer::OnWindowResize()
  {
    Renderer::GetDevice().OnTargetSurfaceImageResized();
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

  GraphicsPipeline& Renderer::GetPipeline()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return *Renderer::Get().m_GraphicsPipeline.get();
  }

  Swapchain& Renderer::GetSwapchain()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return *Renderer::Get().m_Swapchain.get();
  }

  const RendererConfig& Renderer::GetConfig()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return Renderer::Get().m_Config;
  }

}