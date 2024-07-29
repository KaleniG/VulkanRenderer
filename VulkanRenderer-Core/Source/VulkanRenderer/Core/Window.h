#pragma once

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "VulkanRenderer/Events/Event.h"

namespace vkren
{

  class Window
  {
  private:
    using EventCallbackFn = std::function<void(Event&)>;

  public:
    Window() = default;
    Window(const std::string& title, uint32_t width, uint32_t height);
    ~Window();

    void OnUpdate(ImDrawData* imgui_draw_data = nullptr);

    const std::string& GetName() const { return m_WindowData.Title; }
    uint32_t GetWidth() const { return m_WindowData.Width; }
    uint32_t GetHeight() const { return m_WindowData.Height; }

    void SetEventCallback(const EventCallbackFn& callback) { m_WindowData.EventCallback = callback; }

    GLFWwindow* GetNative() { return m_Window; }

  private:
    GLFWwindow* m_Window;
    
    struct WindowData
    {
      std::string Title;
      uint32_t Width;
      uint32_t Height;
      EventCallbackFn EventCallback;
    };

    WindowData m_WindowData;
  };

}