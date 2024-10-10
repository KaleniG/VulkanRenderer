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

    Renderer::Get().m_PipelineCache = PipelineCache::Create("PipelineChache.bin");

    // TEMP
    
    RenderPassStructure renderPassStructure;
    renderPassStructure.NewSubpass();
    renderPassStructure.AddColorAttachment(0, VK_FORMAT_B8G8R8A8_SRGB, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    renderPassStructure.AddDepthStencilAttachment(1, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    renderPassStructure.SubpassDependency
    (
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      0, 
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    );

    Ref<RenderPass> renderPass = RenderPass::Create(renderPassStructure.GetDataAndReset());

    DescriptorSetLayoutConfig setConfig;
    setConfig.Add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    setConfig.Add(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    Ref<DescriptorSetLayout> setLayout = DescriptorSetLayout::Create(setConfig);

    ShaderM vertShader("Assets/Shaders/Shader.vert.spv");
    ShaderM fragShader("Assets/Shaders/Shader.frag.spv");

    PipelineShaders shaders;
    shaders.AddShader(vertShader, "main");
    shaders.AddShader(fragShader, "main");

    PipelineVertexInputState inputState;
    inputState.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
    inputState.AddAttribute(0, VertexInputFormat::VEC3, offsetof(Vertex, Position));
    inputState.AddAttribute(1, VertexInputFormat::VEC3, offsetof(Vertex, Color));
    inputState.AddAttribute(2, VertexInputFormat::VEC2, offsetof(Vertex, TextureCoord));

    PipelineInputAssemblyState inputAssembly(InputPrimitiveTopology::TriangleList);

    PipelineViewportScissorState viewportScissor;

    PipelineRasterizationState rasterizationState(true, false, PolygonMode::Fill, CullMode::Back, FrontFace::CounterClockwise);

    PipelineMultisampleState multisample(MultisampleCount::_1, false);

    DepthTestInfo depthInfo;
    depthInfo.EnableWrite = true;
    depthInfo.CompareOp = CompareOp::Less;
    depthInfo.EnableBoundsTest = false; 

    PipelineDepthStencilState depthStencilState(depthInfo);

    PipelineColorBlendAttachments attachments;
    attachments.AddAttachmentState();

    PipelineColorBlendState colorBlendingState(attachments);

    PipelineDynamicStates dynamicStates;
    dynamicStates.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStates.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);

    PipelineLayoutStructure structure;
    structure.AddDescriptorSetLayout(setLayout);

    Ref<PipelineLayout> pipelineLayout = PipelineLayout::Create(structure);

    GraphicsPipelineM pipeline(shaders, inputState, inputAssembly, viewportScissor, rasterizationState, multisample, depthStencilState, colorBlendingState, dynamicStates, pipelineLayout, renderPass, 0);
    
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

  PipelineCache& Renderer::GetPipelineCache()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return *Renderer::Get().m_PipelineCache.get();
  }

}