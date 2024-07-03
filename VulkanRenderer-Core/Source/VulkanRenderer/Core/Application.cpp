#include <vkrenpch.h>

#include "VulkanRenderer/Core/Application.h"

namespace vkren 
{
  Application::Application(const std::string& name)
    : m_Name(name)
  {

  }

  Application::~Application()
  {

  }

  void Application::Run()
  {
    while (true)
    {
      Timestep timestep = Application::CalculateTimestep();

    }
  }

  Timestep Application::CalculateTimestep()
  {
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
    Timestep timestep = std::chrono::duration<float, std::chrono::seconds::period>(time - m_LastFrameTime).count();
    m_LastFrameTime = time;
    return timestep;
  }

}