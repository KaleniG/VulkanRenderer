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

    void OnEvent(Event& e) override
    {
      EventDispatcher dispatcher(e);
      dispatcher.Dispatch<KeyPressedEvent>(VKREN_BIND_EVENT_FN(MainLayer::OnKeyPressed));
    }

    bool OnKeyPressed(KeyPressedEvent& e)
    {
      switch (e.GetKeyCode())
      {
      case Key::Q:
        Renderer3D::SetRenderMode(RenderMode::Wireframe);
        return true;
      case Key::E:
        Renderer3D::SetRenderMode(RenderMode::Basic);
        return true;
      }

      return false;
    }

    void OnImGuiRender() override
    {
      ImGui::ShowDemoWindow();
    }

  };
}