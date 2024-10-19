#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Timestep.h"
#include "VulkanRenderer/Renderer/EngineComponents/RenderPass.h"
#include "VulkanRenderer/Renderer/Pipeline/DescriptorPool.h"
#include "VulkanRenderer/Renderer/Pipeline/PipelineCache.h"
#include "VulkanRenderer/Renderer/Swapchain.h"

namespace vkren
{

  class Renderer3D
  {
  public:
    static void Init();
    static void Render(Timestep timestep, ImDrawData* imgui_draw_data = nullptr);
    static void Shutdown();

    static const Ref<Swapchain>& GetSwapchain();
    static const Ref<DescriptorPool>& GetDescriptorPool();
    static const Ref<PipelineCache>& GetPipelineCache();
    static const Ref<RenderPass>& GetRenderPass();
  };

}