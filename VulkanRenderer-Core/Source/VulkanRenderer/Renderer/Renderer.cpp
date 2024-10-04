#include <vkrenpch.h>

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Renderer.h"

#include "VulkanRenderer/Renderer/EngineComponents/RenderPass.h"
#include "VulkanRenderer/Renderer/EngineComponents/FrameBuffer.h"
#include "VulkanRenderer/Renderer/Resources/ColorAttachment.h"
#include "VulkanRenderer/Renderer/Resources/DepthStencilAttachment.h"
#include "VulkanRenderer/Renderer/Pipeline/ShaderM.h"
#include "VulkanRenderer/Renderer/Pipeline/Pipeline.h"

namespace vkren
{

  void Renderer::Init(const RendererConfig& config)
  {
    CORE_ASSERT(!Renderer::Get().m_Initialized, "[SYSTEM] Renderer is already initialized");
    Renderer::Get().m_Initialized = true;
    Renderer::Get().m_Config = config;

    Renderer::Get().m_Device = CreateRef<Device>(Renderer::GetConfig().Device);
    Renderer::Get().m_Swapchain = CreateRef<Swapchain>();

    // TEMP

    ShaderM mmmm("Assets/Shaders/Shader.frag.spv");

    struct constDara
    {
      int a;
      int b;
    };

    constDara daar{ 1, 2 };

    PipelineShaders shaders;
    shaders.AddShader(mmmm, "main");
    shaders.AddSpecializationConstantBlock(constDara{1, 1});

    RenderPassStructure structure;
    structure.NewSubpass();
    structure.AddColorAttachment(0, VK_FORMAT_R8G8B8A8_SRGB, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    structure.AddDepthStencilAttachment(1, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    structure.SubpassDependency(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    RenderPassData data = structure.GetDataAndReset();

    ColorAttachmentCreateInfo colorAttachmnetCreateInfo = {};
    colorAttachmnetCreateInfo.Format = VK_FORMAT_R8G8B8A8_SRGB;
    colorAttachmnetCreateInfo.Extent = {1000, 1000};
    Ref<ColorAttachment> colorAttachmnet = ColorAttachment::Create(colorAttachmnetCreateInfo);

    DepthStencilAttachmentCreateInfo dscreateInfo = {};
    dscreateInfo.Format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    dscreateInfo.Extent = { 1000, 1000 };
    Ref<DepthStencilAttachment> depthAttachment = DepthStencilAttachment::Create(dscreateInfo);

    Ref<RenderPass> pass = RenderPass::Create(data);

    FrameBufferStructure fb_stricture(data);
    fb_stricture.AddView(colorAttachmnet);
    fb_stricture.AddView(depthAttachment);

    Ref<FrameBuffer> framebuffer = FrameBuffer::Create(pass, fb_stricture);

    // TEMP
    
    Renderer::Get().m_Texture = CreateRef<Texture>("Assets/Textures/texture.png");

    Renderer::Get().m_UniformBuffers.resize(Renderer::GetDevice().GetConfig().MaxFramesInFlight);
    for (Ref<MUniformBuffer>& buffer : Renderer::Get().m_UniformBuffers)
      buffer = MUniformBuffer::Create(sizeof(UniformBufferObject));

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
    MUniformBuffer& uniformBuffer = *Renderer::Get().m_UniformBuffers[currentFrame].get();
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