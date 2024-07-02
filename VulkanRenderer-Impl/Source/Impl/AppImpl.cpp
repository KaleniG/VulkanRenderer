#include <VulkanRenderer.h>
#include <VulkanRenderer/Core/EntryPoint.h>

class AppImpl : public vkren::Application
{
public:
  AppImpl(const std::string& name)
    : Application(name) {}
};

vkren::Application* vkren::CreateApplication()
{
  return new AppImpl("VulkanRendererImpl");
}