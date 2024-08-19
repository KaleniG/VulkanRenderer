#include <vkrenpch.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/ImGui/ImGuiLayer.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  void ImGuiLayer::OnAttach()
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(Application::GetWindow().GetNative(), true);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = Renderer::GetDevice().GetVulkanInstance();
    initInfo.PhysicalDevice = Renderer::GetDevice().GetPhysical();
    initInfo.Device = Renderer::GetDevice().GetLogical();
    initInfo.RenderPass = Renderer::GetDevice().GetRenderPass();
    initInfo.QueueFamily = Renderer::GetDevice().GetGraphicsQueueFamilyIndex();
    initInfo.Queue = Renderer::GetDevice().GetGraphicsQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = Renderer::GetPipeline().GetDescriptorPool();
    initInfo.Subpass = 0;
    initInfo.MinImageCount = Renderer::GetDevice().GetMinSwapchainImageCount();
    initInfo.ImageCount = Renderer::GetSwapchain().GetImageCount();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = VK_NULL_HANDLE;
    initInfo.CheckVkResultFn = VK_NULL_HANDLE;

    ImGui_ImplVulkan_Init(&initInfo);
  }

  void ImGuiLayer::OnDetach()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiLayer::Start()
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  ImDrawData* ImGuiLayer::Submit()
  {
    ImGui::Render();
    return ImGui::GetDrawData();
  }

}