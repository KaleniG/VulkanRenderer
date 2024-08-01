#include <vkrenpch.h>

#include <VulkanRenderer.h>
#include <VulkanRenderer/Core/EntryPoint.h>

#include "Impl/MainLayer.h"

namespace vkren 
{

  Application* CreateApplication()
  {
    ApplicationConfig appConfig;
    appConfig.Name = "Implementation";
    appConfig.Window.Title = "Implementation";
    appConfig.Window.Width = 1600;
    appConfig.Window.Height = 800;
    appConfig.Renderer.DebugEnabled = true;
    appConfig.Renderer.Device.MaxFramesInFlight = 2;

    Application* impl = new Application(appConfig);
    impl->PushLayer(new MainLayer);

    return impl;
  }
}