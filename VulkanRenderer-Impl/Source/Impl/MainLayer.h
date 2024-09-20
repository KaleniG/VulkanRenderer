#pragma once

#include <VulkanRenderer.h>

namespace vkren
{
  class MainLayer : public Layer
  {
  public:
    MainLayer() : Layer("MainLayer") {}
    ~MainLayer() override {}

    void OnAttach() override
    {

    }

    void OnDetach() override
    {

    }

    void OnUpdate(Timestep timestep) override
    {

    }

    void OnImGuiRender() override
    {
      ImGui::ShowDemoWindow();
    }

  };
}