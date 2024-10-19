#include <vkrenpch.h>

#include <glm/glm.hpp>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Renderer3D.h"

namespace vkren
{

  void Renderer::Init(const RendererConfig& config)
  {
    CORE_ASSERT(!Renderer::Get().m_Initialized, "[SYSTEM] Renderer is already initialized");
    Renderer::Get().m_Initialized = true;
    Renderer::Get().m_Config = config;
    Renderer::Get().m_Device = CreateRef<Device>(Renderer::GetConfig().Device);
    Renderer3D::Init(); 
  }

  void Renderer::Shutdown()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    Renderer3D::Shutdown();
    Renderer::Get().m_Initialized = false;
  }

  void Renderer::DrawFrame(Timestep timestep, ImDrawData* imgui_draw_data)
  {
    Renderer3D::Render(timestep, imgui_draw_data);
  }

  void Renderer::OnExit()
  {
    Renderer::GetDevice().WaitIdle();
  }

  Device& Renderer::GetDevice()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return *Renderer::Get().m_Device.get();
  }

  Ref<Device>& Renderer::GetDeviceRef()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return Renderer::Get().m_Device;
  }

  const RendererConfig& Renderer::GetConfig()
  {
    CORE_ASSERT(Renderer::Get().m_Initialized, "[SYSTEM] Renderer is not initialized");
    return Renderer::Get().m_Config;
  }

}