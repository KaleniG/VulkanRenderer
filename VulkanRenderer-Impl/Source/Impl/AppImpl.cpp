#include <vkrenpch.h>

#include <VulkanRenderer.h>
#include <VulkanRenderer/Core/EntryPoint.h>

#include "Impl/MainLayer.h"

namespace vkren 
{
  class AppImpl : public Application
  {
  public:
    AppImpl(const std::string& name) : Application(name)
    {
      Application::PushLayer(new MainLayer());
    }
  };

  Application* CreateApplication()
  {
    return new AppImpl("VulkanRendererImpl");
  }
}