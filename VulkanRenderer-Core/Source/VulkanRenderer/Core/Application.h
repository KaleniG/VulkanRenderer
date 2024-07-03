#pragma once

#include <string>
#include <chrono>

#include "VulkanRenderer/Core/LayerStack.h"
#include "VulkanRenderer/Core/Timestep.h"

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

  private:
    std::string m_Name;
    LayerStack m_LayerStack;
    std::chrono::steady_clock::time_point m_LastFrameTime;

  private:
    static Application* s_Instance;
  };

  Application* CreateApplication();

}