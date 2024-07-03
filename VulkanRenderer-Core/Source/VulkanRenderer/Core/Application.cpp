#include <vkrenpch.h>

#include "VulkanRenderer/Core/Application.h"

namespace vkren 
{
  Application::Application(const std::string& name)
    : m_Name(name), m_Window(m_Name, 1600, 800)
  {
    m_Window.SetEventCallback(VKREN_BIND_EVENT_FN(Application::OnEvent));
  }

  Application::~Application()
  {

  }

  void Application::PushLayer(Layer* layer)
  {
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PushOverlay(Layer* overlay)
  {
    m_LayerStack.PushOverlay(overlay);
    overlay->OnAttach();
  }

  void Application::OnEvent(Event& e)
  {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(VKREN_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(VKREN_BIND_EVENT_FN(Application::OnWindowResize));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
    {
      if (e.Handled)
        break;
      (*it)->OnEvent(e);
    }
  }

  void Application::Run()
  {
    while (m_IsRunning)
    {
      Timestep timestep = Application::CalculateTimestep();

      if (!m_IsMinimized)
      {
        for (Layer* layer : m_LayerStack)
          layer->OnUpdate(timestep);
      }
      
      m_Window.OnUpdate();
    }
  }

  Timestep Application::CalculateTimestep()
  {
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
    Timestep timestep = std::chrono::duration<float, std::chrono::seconds::period>(time - m_LastFrameTime).count();
    m_LastFrameTime = time;
    return timestep;
  }

  bool Application::OnWindowClose(WindowCloseEvent& e)
  {
    m_IsRunning = false;
    return true;
  }

  bool Application::OnWindowResize(WindowResizeEvent& e)
  {
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
      m_IsMinimized = true;
      return false;
    }

    m_IsMinimized = false;

    //Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
  }

}