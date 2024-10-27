#include <vkrenpch.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <backends/imgui_impl_vulkan.h>

#include "VulkanRenderer/Renderer/EngineComponents/CommandBuffer.h"
#include "VulkanRenderer/Renderer/EngineComponents/CommandPool.h"
#include "VulkanRenderer/Renderer/EngineComponents/RenderPass.h"
#include "VulkanRenderer/Renderer/EngineComponents/Semaphore.h"
#include "VulkanRenderer/Renderer/EngineComponents/Fence.h"
#include "VulkanRenderer/Renderer/Resources/MUniformBuffer.h"
#include "VulkanRenderer/Renderer/Resources/VertexBuffer.h"
#include "VulkanRenderer/Renderer/Resources/IndexBuffer.h"
#include "VulkanRenderer/Renderer/Resources/Texture.h"
#include "VulkanRenderer/Renderer/Resources/Model.h"
#include "VulkanRenderer/Renderer/Pipeline/DescriptorPool.h"
#include "VulkanRenderer/Renderer/Pipeline/PipelineCache.h"
#include "VulkanRenderer/Renderer/Pipeline/DescriptorSet.h"
#include "VulkanRenderer/Renderer/Pipeline/Pipeline.h"
#include "VulkanRenderer/Renderer/Pipeline/Shader.h"
#include "VulkanRenderer/Renderer/Renderer3D.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"
#include "VulkanRenderer/Renderer/Vertex.h"

#include "VulkanRenderer/GameComponents/Map/Terrain.h"
#include "VulkanRenderer/GameComponents/Camera.h"

#define MAX_FRAMES_IN_FLIGHT 2

namespace vkren
{

  struct Renderer3DData
  {
    // TESTING...
    bool EnableWireframe = false;
    Camera Camera;

    /* TERRAIN TEST
    Terrain terrain;
    */
    Ref<PipelineCache> PipelineCache;

    Ref<CommandPool> CommandPool;
    std::array<Ref<CommandBuffer>, MAX_FRAMES_IN_FLIGHT> CommandBuffers;
    std::array<Ref<Fence>, MAX_FRAMES_IN_FLIGHT> InFlightFences;
    std::array<Ref<Semaphore>, MAX_FRAMES_IN_FLIGHT> RenderFinishedSemaphores;
    std::array<Ref<Semaphore>, MAX_FRAMES_IN_FLIGHT> ImageAvailableSemaphores;

    uint32_t CurrentFrame = 0;
    uint32_t CurrentImageIndex = 0;
    Ref<VertexBuffer> VertexBuffer;
    Ref<IndexBuffer> IndexBuffer;

    Ref<RenderPass> MainRenderPass;
    Ref<Swapchain> Swapchain;
    Ref<DescriptorPool> DescriptorPool;
    std::array<Ref<MUniformBuffer>, MAX_FRAMES_IN_FLIGHT> UniformBuffers;

    std::array<Ref<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> BasicPipelineDescriptorSets;
    Ref<DescriptorSetLayout> BasicDescriptorSetLayout;
    Ref<Shader> BasicVertShader;
    Ref<Shader> BasicFragShader;
    Ref<GraphicsPipelineM> BasicPipeline;
    Ref<PipelineLayout> BasicPipelineLayout;

    std::array<Ref<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> WireframePipelineDescriptorSets;
    Ref<DescriptorSetLayout> WireframeDescriptorSetLayout;
    Ref<Shader> WireframeVertShader;
    Ref<Shader> WireframeFragShader;
    Ref<GraphicsPipelineM> WireframePipeline;
    Ref<PipelineLayout> WireframePipelineLayout;

    Texture DozerTexture;
    Ref<Model> DozerModel;
  };

  static Renderer3DData* s_Data = new Renderer3DData();

  void Renderer3D::Init()
  {
    // TEST
    s_Data->Camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.001f);

    /* TERRAIN TEST
    {
      s_Data->terrain = Terrain(glm::uvec2(10, 10), 8, nullptr);
      s_Data->terrain.RaiseTerrain(glm::ivec2(4, 4), 0, 3, 20);
    }
    */

    // PIPELINE CACHE CREATION
    {
      s_Data->PipelineCache = PipelineCache::Create("PipelineChache.pcache");
    }

    // COMMAND POOL AND COMMAND BUFFERS CREATION
    {
      s_Data->CommandPool = CommandPool::Create(Renderer::GetDevice().GetGraphicsQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
      
      for (Ref<CommandBuffer>& cb : s_Data->CommandBuffers)
        cb = CommandBuffer::Allocate(s_Data->CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    }

    // SYNCHRONIZATION OBJECTS CREATION
    {
      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
      {
        s_Data->InFlightFences[i] = Fence::Create(VK_FENCE_CREATE_SIGNALED_BIT);
        s_Data->RenderFinishedSemaphores[i] = Semaphore::Create();
        s_Data->ImageAvailableSemaphores[i] = Semaphore::Create();
      }
    }

    // RENDERPASS CREATION
    {
      RenderPassStructure renderPassStructure;
      renderPassStructure.NewSubpass();
      renderPassStructure.AddColorAttachment(0, VK_FORMAT_B8G8R8A8_SRGB, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
      renderPassStructure.AddDepthStencilAttachment(1, VK_FORMAT_D32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
      renderPassStructure.SubpassDependency
      (
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
      );

      s_Data->MainRenderPass = RenderPass::Create(renderPassStructure.GetDataAndReset());
    }

    // SWAPCHAIN CREATION
    {
      s_Data->Swapchain = Swapchain::Create(s_Data->MainRenderPass);
    }

    // DESCRIPTOR SET LAYOUTS & POOL CREATION
    {
      DescriptorSetLayoutConfig basicSetLayout;
      basicSetLayout.Add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
      basicSetLayout.Add(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
      s_Data->BasicDescriptorSetLayout = DescriptorSetLayout::Create(basicSetLayout);

      DescriptorSetLayoutConfig wireframeSetLayout;
      wireframeSetLayout.Add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
      s_Data->WireframeDescriptorSetLayout = DescriptorSetLayout::Create(wireframeSetLayout);

      s_Data->DescriptorPool = DescriptorPool::Create({ s_Data->BasicDescriptorSetLayout, s_Data->WireframeDescriptorSetLayout }, true);
    }

    // DESCRIPTOR SETS CREATION
    {
      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
      {
        s_Data->BasicPipelineDescriptorSets[i] = DescriptorSet::Allocate(s_Data->BasicDescriptorSetLayout, s_Data->DescriptorPool);
        s_Data->WireframePipelineDescriptorSets[i] = DescriptorSet::Allocate(s_Data->WireframeDescriptorSetLayout, s_Data->DescriptorPool);
      }
    }

    // SHADERS CREATION
    {
      s_Data->BasicVertShader = Shader::Create("Assets/Shaders/Basic.vert.spv");
      s_Data->BasicFragShader = Shader::Create("Assets/Shaders/Basic.frag.spv");
      s_Data->WireframeVertShader = Shader::Create("Assets/Shaders/Wireframe.vert.spv");
      s_Data->WireframeFragShader = Shader::Create("Assets/Shaders/Wireframe.frag.spv");
    }

    // PIPELINE LAYOUTS CREATION
    {
      PipelineLayoutStructure basicLayoutStructure;
      basicLayoutStructure.AddDescriptorSetLayout(s_Data->BasicDescriptorSetLayout);

      s_Data->BasicPipelineLayout = PipelineLayout::Create(basicLayoutStructure);

      PipelineLayoutStructure wireframeLayoutStructure;
      wireframeLayoutStructure.AddDescriptorSetLayout(s_Data->WireframeDescriptorSetLayout);

      s_Data->WireframePipelineLayout = PipelineLayout::Create(wireframeLayoutStructure);
    }

    // BASIC PIPELINE CREATION
    {
      PipelineShaders pShaderStages;
      pShaderStages.AddShader(s_Data->BasicVertShader, "main");
      pShaderStages.AddShader(s_Data->BasicFragShader, "main");

      PipelineVertexInputState pInputState;
      pInputState.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
      pInputState.AddAttribute(0, VertexInputFormat::VEC3, offsetof(Vertex, Position));
      pInputState.AddAttribute(1, VertexInputFormat::VEC3, offsetof(Vertex, Color));
      pInputState.AddAttribute(2, VertexInputFormat::VEC2, offsetof(Vertex, TextureCoord));

      PipelineInputAssemblyState pInputAssembly(InputPrimitiveTopology::TriangleList);

      PipelineViewportScissorState pViewportScissorState;

      PipelineRasterizationState pRasterizationState(true, false, PolygonMode::Fill, CullMode::Back, FrontFace::CounterClockwise);

      PipelineMultisampleState pMultisampleState(MultisampleCount::_1, false);

      DepthTestInfo depthTestInfo;
      depthTestInfo.EnableWrite = true;
      depthTestInfo.CompareOp = CompareOp::Less;
      depthTestInfo.EnableBoundsTest = false;

      PipelineDepthStencilState pDepthStencilState(depthTestInfo);

      PipelineColorBlendAttachments colorBlendAttachments;
      colorBlendAttachments.AddAttachmentState();

      PipelineColorBlendState pColorBlendingState(colorBlendAttachments);

      PipelineDynamicStates pDynamicStates;
      pDynamicStates.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
      pDynamicStates.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);

      s_Data->BasicPipeline = GraphicsPipelineM::Create
      (
        pShaderStages,
        pInputState,
        pInputAssembly,
        pViewportScissorState,
        pRasterizationState,
        pMultisampleState,
        pDepthStencilState,
        pColorBlendingState,
        pDynamicStates,
        s_Data->BasicPipelineLayout,
        s_Data->MainRenderPass,
        0
      );
    }

    // WIREFRAME PIPELINE CREATION
    {
      PipelineShaders pShaderStages;
      pShaderStages.AddShader(s_Data->WireframeVertShader, "main");
      pShaderStages.AddShader(s_Data->WireframeFragShader, "main");

      PipelineVertexInputState pInputState;
      pInputState.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
      pInputState.AddAttribute(0, VertexInputFormat::VEC3, offsetof(Vertex, Position));
      pInputState.AddAttribute(1, VertexInputFormat::VEC3, offsetof(Vertex, Color));
      pInputState.AddAttribute(2, VertexInputFormat::VEC2, offsetof(Vertex, TextureCoord));

      PipelineInputAssemblyState pInputAssembly(InputPrimitiveTopology::TriangleList);

      PipelineViewportScissorState pViewportScissorState;

      PipelineRasterizationState pRasterizationState(true, false, PolygonMode::Line, CullMode::Back, FrontFace::CounterClockwise);

      PipelineMultisampleState pMultisampleState(MultisampleCount::_1, false);

      DepthTestInfo depthTestInfo;
      depthTestInfo.EnableWrite = true;
      depthTestInfo.CompareOp = CompareOp::Less;
      depthTestInfo.EnableBoundsTest = false;

      PipelineDepthStencilState pDepthStencilState(depthTestInfo);

      PipelineColorBlendAttachments colorBlendAttachments;
      colorBlendAttachments.AddAttachmentState();

      PipelineColorBlendState pColorBlendingState(colorBlendAttachments);

      PipelineDynamicStates pDynamicStates;
      pDynamicStates.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
      pDynamicStates.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);

      s_Data->WireframePipeline = GraphicsPipelineM::Create
      (
        pShaderStages,
        pInputState,
        pInputAssembly,
        pViewportScissorState,
        pRasterizationState,
        pMultisampleState,
        pDepthStencilState,
        pColorBlendingState,
        pDynamicStates,
        s_Data->WireframePipelineLayout,
        s_Data->MainRenderPass,
        0
      );
    }

    // DOZER TEXTURE
    {
      s_Data->DozerTexture = Texture::Create("Assets/Textures/USA_DOZER.png");
    }

    // UNIFORM BUFFERS
    {
      for (Ref<MUniformBuffer>& ub : s_Data->UniformBuffers)
        ub = MUniformBuffer::Create(sizeof(ModelViewProjectionUBO));
    }

    // MODEL LOADING
    {
      s_Data->DozerModel = Model::Create("Assets/Models/USA_DOZER_NEW.obj");
    }

    // VERTEX & INDEX BUFFER POPULATION
    {
      s_Data->VertexBuffer = VertexBuffer::Create(s_Data->DozerModel->GetVertices());
      s_Data->IndexBuffer = IndexBuffer::Create(s_Data->DozerModel->GetIndices());
    }

    // DESCRIPTOR SETS POPULATION
    {
      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
      {
        DescriptorSetUpdateData basicPipelineUpdateData = {};
        basicPipelineUpdateData.Write(0, s_Data->UniformBuffers[i]);
        basicPipelineUpdateData.Write(1, s_Data->DozerTexture.GetImage());

        s_Data->BasicPipelineDescriptorSets[i]->Update(basicPipelineUpdateData);

        DescriptorSetUpdateData wireframePipelineUpdateData = {};
        wireframePipelineUpdateData.Write(0, s_Data->UniformBuffers[i]);

        s_Data->WireframePipelineDescriptorSets[i]->Update(wireframePipelineUpdateData);
      }
    }

  }

  void Renderer3D::Render(Timestep timestep, ImDrawData* imgui_draw_data)
  {
    s_Data->Camera.OnUpdate(timestep);

    s_Data->InFlightFences[s_Data->CurrentFrame]->Wait();

    s_Data->CurrentImageIndex = s_Data->Swapchain->AcquireNextImage(s_Data->ImageAvailableSemaphores[s_Data->CurrentFrame]);
    if (s_Data->CurrentImageIndex == -1)
    {
      s_Data->CurrentFrame = (s_Data->CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
      return;
    }

    s_Data->InFlightFences[s_Data->CurrentFrame]->Reset();

    ModelViewProjectionUBO ubo;
    ubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
    ubo.view = s_Data->Camera.GetView(); //glm::lookAt(glm::vec3(5.0f, 5.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(s_Data->Camera.GetZoom()), s_Data->Swapchain->GetExtent().width / static_cast<float>(s_Data->Swapchain->GetExtent().height), 0.1f, 100.0f);
    ubo.proj[1][1] *= -1;

    s_Data->UniformBuffers[s_Data->CurrentFrame]->Update(&ubo);

    /////////////////////////////////////
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    VkResult result = vkBeginCommandBuffer(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), &commandBufferBeginInfo);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to begin recording command buffer. Error: {}", Utils::VkResultToString(result));

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = s_Data->MainRenderPass->Get();
    renderPassBeginInfo.framebuffer = s_Data->Swapchain->GetFramebuffers()[s_Data->CurrentImageIndex];
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = s_Data->Swapchain->GetExtent();
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(s_Data->Swapchain->GetExtent().width);
    viewport.height = static_cast<float>(s_Data->Swapchain->GetExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = s_Data->Swapchain->GetExtent();
    vkCmdSetScissor(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), 0, 1, &scissor);

    VkBuffer vertexBuffers[] = { s_Data->VertexBuffer->Get() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), s_Data->IndexBuffer->Get(), 0, VK_INDEX_TYPE_UINT32);
    
    if (s_Data->EnableWireframe)
    {
      vkCmdBindDescriptorSets(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data->WireframePipelineLayout->Get(), 0, 1, &s_Data->WireframePipelineDescriptorSets[s_Data->CurrentFrame]->Get(), 0, VK_NULL_HANDLE);
      vkCmdBindPipeline(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data->WireframePipeline->Get());
    }
    else
    {
      vkCmdBindDescriptorSets(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data->BasicPipelineLayout->Get(), 0, 1, &s_Data->BasicPipelineDescriptorSets[s_Data->CurrentFrame]->Get(), 0, VK_NULL_HANDLE);
      vkCmdBindPipeline(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data->BasicPipeline->Get());
    }

    vkCmdDrawIndexed(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), static_cast<uint32_t>(s_Data->IndexBuffer->GetIndexCount()), 1, 0, 0, 0);

    if (imgui_draw_data)
      ImGui_ImplVulkan_RenderDrawData(imgui_draw_data, s_Data->CommandBuffers[s_Data->CurrentFrame]->Get());

    vkCmdEndRenderPass(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get());
    result = vkEndCommandBuffer(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get());
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to end recording command buffer. Error: {}", Utils::VkResultToString(result));

    /////////////////////////////////////

    s_Data->CommandBuffers[s_Data->CurrentFrame]->Submit
    (
      s_Data->ImageAvailableSemaphores[s_Data->CurrentFrame],
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      s_Data->RenderFinishedSemaphores[s_Data->CurrentFrame],
      s_Data->InFlightFences[s_Data->CurrentFrame]
    );

    s_Data->Swapchain->Present
    (
      s_Data->CurrentImageIndex, 
      s_Data->RenderFinishedSemaphores[s_Data->CurrentFrame]
    );

    s_Data->CurrentFrame = (s_Data->CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void Renderer3D::Shutdown()
  {
    delete s_Data;
  }

  void Renderer3D::SetRenderMode(RenderMode mode)
  {
    switch (mode)
    {
      case RenderMode::Basic:
        s_Data->EnableWireframe = false;
        break;
      case RenderMode::Wireframe:
        s_Data->EnableWireframe = true;
        break;
    }
  }

  const Ref<Swapchain>& Renderer3D::GetSwapchain()
  {
    return s_Data->Swapchain;
  }

  const Ref<DescriptorPool>& Renderer3D::GetDescriptorPool()
  {
    return s_Data->DescriptorPool;
  }

  const Ref<PipelineCache>& Renderer3D::GetPipelineCache()
  {
    return s_Data->PipelineCache;
  }

  const Ref<RenderPass>& Renderer3D::GetRenderPass()
  {
    return s_Data->MainRenderPass;
  }

}