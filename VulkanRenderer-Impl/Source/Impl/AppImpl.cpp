#include <vkrenpch.h>

#include <VulkanRenderer.h>
#include <VulkanRenderer/Core/EntryPoint.h>

#include "Impl/MainLayer.h"

namespace vkren 
{
  class AppImpl : public Application
  {
  public:
    AppImpl(const std::string& name, uint32_t window_width, uint32_t window_height) 
      : Application(name, window_width, window_height)
    {
      Application::PushLayer(new MainLayer());
    }
  };

  Application* CreateApplication()
  {
    return new AppImpl("VulkanRendererImpl", 1600, 800);
  }
}