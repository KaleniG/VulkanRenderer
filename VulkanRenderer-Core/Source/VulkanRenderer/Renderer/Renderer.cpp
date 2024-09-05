#include <vkrenpch.h>

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Renderer.h"

#include "VulkanRenderer/Renderer/Resources/Image.h"
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

    /*
    ImageCreateInfo imgInfo = {};
    imgInfo.Type = VK_IMAGE_TYPE_2D;
    imgInfo.Extent = { 10, 10, 1 };
    imgInfo.Format = VK_FORMAT_R8G8B8A8_SRGB;
    imgInfo.LayerCount = 1;
    imgInfo.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    imgInfo.MipmapLevels = 1;
    imgInfo.SampleCount = VK_SAMPLE_COUNT_1_BIT;
    imgInfo.Tiling = VK_IMAGE_TILING_OPTIMAL;
    imgInfo.Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    Image imag1 = Image::Create(imgInfo);
    Image imag2 = Image::Create(imgInfo);
    imag1.CopyToImage(imag2);
    imag2.CopyToImage(imag1);

    BufferCreateInfo buffInfo = {};
    buffInfo.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffInfo.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffInfo.Size = imgInfo.Extent.height * imgInfo.Extent.width * imgInfo.Extent.depth * 4;

    Buffer buff1 = Buffer::Create(buffInfo);
    imag1.CopyToBuffer(buff1);
    imag2.CopyToBuffer(buff1);
    buff1.CopyToImage(imag1);
    buff1.CopyToImage(imag2);
    */

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