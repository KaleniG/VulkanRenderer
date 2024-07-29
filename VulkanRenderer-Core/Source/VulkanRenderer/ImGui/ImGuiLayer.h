#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "VulkanRenderer/Core/Layer.h"

namespace vkren
{

  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer() : Layer("ImGui") {}

    void OnAttach() override;
    void OnDetach() override;

    void Start();
    ImDrawData* Submit();
  };

}