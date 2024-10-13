#include <vkrenpch.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <backends/imgui_impl_vulkan.h>

#include "VulkanRenderer/Renderer/DescriptorSet/DescriptorPool.h"
#include "VulkanRenderer/Renderer/DescriptorSet/DescriptorSet.h"
#include "VulkanRenderer/Renderer/EngineComponents/CommandBuffer.h"
#include "VulkanRenderer/Renderer/EngineComponents/CommandPool.h"
#include "VulkanRenderer/Renderer/EngineComponents/RenderPass.h"
#include "VulkanRenderer/Renderer/EngineComponents/Semaphore.h"
#include "VulkanRenderer/Renderer/EngineComponents/Fence.h"
#include "VulkanRenderer/Renderer/Resources/MUniformBuffer.h"
#include "VulkanRenderer/Renderer/Resources/VertexBufferM.h"
#include "VulkanRenderer/Renderer/Resources/IndexBufferM.h"
#include "VulkanRenderer/Renderer/Resources/TextureM.h"
#include "VulkanRenderer/Renderer/Pipeline/PipelineCache.h"
#include "VulkanRenderer/Renderer/Pipeline/Pipeline.h"
#include "VulkanRenderer/Renderer/Pipeline/ShaderM.h"
#include "VulkanRenderer/Renderer/Renderer3D.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"
#include "VulkanRenderer/Renderer/Model.h"

#define MAX_FRAMES_IN_FLIGHT 2

namespace vkren
{
  
  struct alignas(16) ModelViewProjectionUBO
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  struct alignas(16) Vertex
  {
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 TextureCoord;
  };

  struct Renderer3DData
  {
    Ref<PipelineCache> PipelineCache;

    Ref<CommandPool> CommandPool;
    std::array<Ref<CommandBuffer>, MAX_FRAMES_IN_FLIGHT> CommandBuffers;
    std::array<Ref<Fence>, MAX_FRAMES_IN_FLIGHT> InFlightFences;
    std::array<Ref<Semaphore>, MAX_FRAMES_IN_FLIGHT> RenderFinishedSemaphores;
    std::array<Ref<Semaphore>, MAX_FRAMES_IN_FLIGHT> ImageAvailableSemaphores;

    uint32_t CurrentFrame = 0;
    uint32_t CurrentImageIndex = 0;
    Ref<VertexBufferM> VertexBuffer;
    Ref<IndexBufferM> IndexBuffer;

    Ref<RenderPass> MainRenderPass;
    Ref<Swapchain> Swapchain;
    Ref<DescriptorSetLayout> MainDescriptorSetLayout;
    Ref<DescriptorPool> DescriptorPool;
    std::array<Ref<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> DescriptorSets;
    std::array<Ref<MUniformBuffer>, MAX_FRAMES_IN_FLIGHT> UniformBuffers;
    Ref<ShaderM> VertShader;
    Ref<ShaderM> FragShader;
    Ref<GraphicsPipelineM> MainPipeline;
    Ref<PipelineLayout> MainPipelineLayout;

    TextureM DozerTexture;
    Ref<Model> DozerModel;
  };

  static Renderer3DData* s_Data = new Renderer3DData();

  void Renderer3D::Init()
  {
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

    // DESCRIPTOR SET LAYOUT & POOL CREATION
    {
      DescriptorSetLayoutConfig setConfig;
      setConfig.Add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
      setConfig.Add(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
      s_Data->MainDescriptorSetLayout = DescriptorSetLayout::Create(setConfig);

      s_Data->DescriptorPool = DescriptorPool::Create({ s_Data->MainDescriptorSetLayout }, true);
    }

    // DESCRIPTOR SET CREATION
    {
      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        s_Data->DescriptorSets[i] = DescriptorSet::Allocate(s_Data->MainDescriptorSetLayout, s_Data->DescriptorPool);
    }

    // SHADERS CREATION
    {
      s_Data->VertShader = ShaderM::Create("Assets/Shaders/Shader.vert.spv");
      s_Data->FragShader = ShaderM::Create("Assets/Shaders/Shader.frag.spv");
    }

    // PIPELINE LAYOUT CREATION
    {
      PipelineLayoutStructure layoutStructure;
      layoutStructure.AddDescriptorSetLayout(s_Data->MainDescriptorSetLayout);

      s_Data->MainPipelineLayout = PipelineLayout::Create(layoutStructure);
    }

    // PIPELINE CREATION
    {
      PipelineShaders pShaderStages;
      pShaderStages.AddShader(s_Data->VertShader, "main");
      pShaderStages.AddShader(s_Data->FragShader, "main");

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

      s_Data->MainPipeline = GraphicsPipelineM::Create
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
        s_Data->MainPipelineLayout,
        s_Data->MainRenderPass,
        0
      );
    }

    // DOZER TEXTURE
    {
      s_Data->DozerTexture = TextureM::Create("Assets/Textures/USA_DOZER.png");
    }

    // UNIFORM BUFFERS
    {
      for (Ref<MUniformBuffer>& ub : s_Data->UniformBuffers)
        ub = MUniformBuffer::Create(sizeof(ModelViewProjectionUBO));
    }

    // MODEL LOADING
    {
      s_Data->DozerModel = Model::Create("Assets/Models/USA_DOZER_NEW.obj", "Assets/Models/USA_DOZER_NEW.mtl");
    }

    // VERTEX & INDEX BUFFER POPULATION
    {
      s_Data->VertexBuffer = VertexBufferM::Create(s_Data->DozerModel->GetVertices());
      s_Data->IndexBuffer = IndexBufferM::Create(s_Data->DozerModel->GetIndices());
    }

    // DESCRIPTOR SETS POPULATION
    {
      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
      {
        DescriptorSetUpdateData updateData = {};
        updateData.Write(0, s_Data->UniformBuffers[i]);
        updateData.Write(1, s_Data->DozerTexture.GetImage());

        s_Data->DescriptorSets[i]->Update(updateData);
      }
    }

  }

  void Renderer3D::Render(Timestep timestep, ImDrawData* imgui_draw_data)
  {
    s_Data->InFlightFences[s_Data->CurrentFrame]->Wait();

    s_Data->CurrentImageIndex = s_Data->Swapchain->AcquireNextImage(s_Data->ImageAvailableSemaphores[s_Data->CurrentFrame]);
    if (s_Data->CurrentImageIndex == -1)
    {
      s_Data->CurrentFrame = (s_Data->CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
      return;
    }

    s_Data->InFlightFences[s_Data->CurrentFrame]->Reset();

    ModelViewProjectionUBO ubo;
    ubo.model = glm::rotate(glm::mat4(1.0f), timestep * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.model = glm::scale(ubo.model, glm::vec3(0.05f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), s_Data->Swapchain->GetExtent().width / static_cast<float>(s_Data->Swapchain->GetExtent().height), 0.1f, 10.0f);
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

    vkCmdBindPipeline(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data->MainPipeline->Get());

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
    vkCmdBindDescriptorSets(s_Data->CommandBuffers[s_Data->CurrentFrame]->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_Data->MainPipelineLayout->Get(), 0, 1, &s_Data->DescriptorSets[s_Data->CurrentFrame]->Get(), 0, VK_NULL_HANDLE);

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