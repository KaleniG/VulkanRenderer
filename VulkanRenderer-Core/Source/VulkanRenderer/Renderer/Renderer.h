#pragma once

#include <imgui.h>

#include "VulkanRenderer/Renderer/Device.h"

#include "VulkanRenderer/Renderer/Pipeline/PipelineCache.h"

namespace vkren
{

  struct RendererConfig
  {
    bool DebugEnabled;
    DeviceConfig Device;
  };

  class Renderer
  {
  public:
    Renderer(const Renderer&) = delete;
    Renderer& operator = (const Renderer&) = delete;

    static void Init(const RendererConfig& config);
    static void Shutdown();
    static void DrawFrame(Timestep timestep, ImDrawData* imgui_draw_data = nullptr);
    static void OnExit();

    static Device& GetDevice();
    static Ref<Device>& GetDeviceRef();
    static const RendererConfig& GetConfig();

  private:
    Renderer() = default;
    static Renderer& Get() { static Renderer instance; return instance; }

  private:
    bool m_Initialized = false;
    RendererConfig m_Config;
    Ref<Device> m_Device;
  };

}