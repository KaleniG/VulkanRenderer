#pragma once

#include <string>

namespace vkren 
{

  class Application 
  {
  public:
    Application(const std::string& name);
    ~Application();

    void Run();

  private:
    std::string m_Name;

  private:
    static Application* s_Instance;
  };

  Application* CreateApplication();

}