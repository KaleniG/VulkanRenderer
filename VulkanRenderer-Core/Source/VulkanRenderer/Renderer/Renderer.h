#pragma once

#include <imgui.h>

#include "VulkanRenderer/Renderer/Resources/MUniformBuffer.h"
#include "VulkanRenderer/Renderer/GraphicsPipeline.h"
#include "VulkanRenderer/Renderer/VertexBuffer.h"
#include "VulkanRenderer/Renderer/IndexBuffer.h"
#include "VulkanRenderer/Renderer/Swapchain.h"
#include "VulkanRenderer/Renderer/Texture.h"
#include "VulkanRenderer/Renderer/Device.h"
#include "VulkanRenderer/Renderer/Shader.h"
#include "VulkanRenderer/Renderer/Model.h"

#include "VulkanRenderer/Renderer/Pipeline/PipelineCache.h"

namespace vkren
{

  struct alignas(16) UniformBufferObject
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  struct RendererConfig
  {
    bool DebugEnabled;
    DeviceConfig Device;
  };

  class Renderer
  {
  public:
    Renderer(const Renderer&) = delete;
    Renderer& operator = (const Renderer&) = delete;

    static void Init(const RendererConfig& config);
    static void Shutdown();
    static void DrawFrame(ImDrawData* imgui_draw_data = nullptr);
    static void OnExit();
    static void OnWindowResize();

    static Device& GetDevice();
    static Ref<Device>& GetDeviceRef();
    static GraphicsPipeline& GetPipeline();
    static Swapchain& GetSwapchain();
    static const RendererConfig& GetConfig();

    static PipelineCache& GetPipelineCache();

  private:
    Renderer() = default;
    static Renderer& Get() { static Renderer instance; return instance; }

  private:
    bool m_Initialized = false;
    RendererConfig m_Config;

    Ref<Device> m_Device;
    Ref<Swapchain> m_Swapchain;

    Ref<PipelineCache> m_PipelineCache;

    // TEMP
    Ref<Shader> m_Shader;
    Ref<GraphicsPipeline> m_GraphicsPipeline;

    Ref<Texture> m_Texture;
    std::vector<Ref<MUniformBuffer>> m_UniformBuffers;

    Ref<Model> m_Model;
    Ref<VertexBuffer> m_VertexBuffer;
    Ref<IndexBuffer> m_IndexBuffer;

    uint32_t m_CurrentFrame;
    // TEMP
  };

}