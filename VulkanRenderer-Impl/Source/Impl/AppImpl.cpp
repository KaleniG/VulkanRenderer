#include <VulkanRenderer.h>
#include <VulkanRenderer/Core/EntryPoint.h>

#include "Impl/MainLayer.h"

class AppImpl : public vkren::Application
{
public:
  AppImpl(const std::string& name) : Application(name) 
  {
    Application::PushLayer(new MainLayer());
  }
};

vkren::Application* vkren::CreateApplication()
{
  return new AppImpl("VulkanRendererImpl");
}