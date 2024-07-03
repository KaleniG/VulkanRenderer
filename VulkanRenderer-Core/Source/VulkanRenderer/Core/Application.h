#pragma once

#include <string>
#include <chrono>

#include "VulkanRenderer/Core/LayerStack.h"
#include "VulkanRenderer/Core/Timestep.h"
#include "VulkanRenderer/Core/Window.h"
#include "VulkanRenderer/Events/ApplicationEvent.h"

namespace vkren 
{

  class Application 
  {
  public:
    Application(const std::string& name);
    ~Application();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    void OnEvent(Event& e);

    void Run();

  private:
    Timestep CalculateTimestep();

    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

  private:
    std::string m_Name;
    Window m_Window;

    LayerStack m_LayerStack;
    std::chrono::steady_clock::time_point m_LastFrameTime;

    bool m_IsRunning = true;
    bool m_IsMinimized = false;

  private:
    static Application* s_Instance;
  };

  Application* CreateApplication();

}