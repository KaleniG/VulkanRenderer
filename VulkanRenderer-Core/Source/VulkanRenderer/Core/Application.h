#pragma once

#include "VulkanRenderer/Core/LayerStack.h"
#include "VulkanRenderer/Core/Timestep.h"
#include "VulkanRenderer/Core/Window.h"
#include "VulkanRenderer/Events/ApplicationEvent.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren 
{

  class Application 
  {
  public:
    Application(const std::string& name, uint32_t window_width, uint32_t window_height);
    ~Application();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    void OnEvent(Event& e);

    void Run();

    Window& GetWindow() { return m_Window; }
    const std::string& GetName() { return m_Name; }
    static Application& Get() { return *s_ApplicationInstance; }

  private:
    Timestep CalculateTimestep();

    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

  private:
    std::string m_Name;
    Window m_Window;
    Scope<Renderer> m_Renderer;

    LayerStack m_LayerStack;
    std::chrono::steady_clock::time_point m_LastFrameTime;

    bool m_IsRunning = true;
    bool m_IsMinimized = false;

  private:
    static Application* s_ApplicationInstance;
  };

  Application* CreateApplication();

}