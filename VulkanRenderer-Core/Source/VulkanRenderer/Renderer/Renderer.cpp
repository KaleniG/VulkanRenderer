#include <vkrenpch.h>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/DebugMessenger.h"

namespace vkren
{

  struct RendererRequirements
  {
    std::vector<const char*> Layers =
    {
      #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
      "VK_LAYER_KHRONOS_validation"
      #endif
    };

    std::vector<const char*> InstanceExtensions =
    {
      "VK_KHR_surface",
      "VK_KHR_win32_surface",
      #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
      "VK_EXT_debug_utils"
      #endif
    };

    std::vector<const char*> DeviceExtensions =
    {
      "VK_KHR_swapchain"
    };

  };

  static RendererRequirements* s_RendererRequirements = new RendererRequirements;

  void Renderer::Init()
  {
    m_WindowReference = &Application::Get().GetWindow();

    Renderer::CreateVulkanInstance();
    Renderer::CreateDebugMessenger();
  }

  void Renderer::Shutdown()
  {
    Renderer::DestroyDebugMessenger();
    Renderer::DestroyVulkanInstance();
  }

  void Renderer::CreateVulkanInstance()
  {
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "VulkanTutorial";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "None";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(s_RendererRequirements->InstanceExtensions.size());
    create_info.ppEnabledExtensionNames = s_RendererRequirements->InstanceExtensions.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(s_RendererRequirements->Layers.size());
    create_info.ppEnabledLayerNames = s_RendererRequirements->Layers.data();
    #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = vkdebug::debug_utils_messenger_callback;
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_messenger_create_info;
    #endif

    VkResult result_vk = vkCreateInstance(&create_info, nullptr, &m_VulkanInstance);
    if (result_vk != VK_SUCCESS)
      return;
  }

  void Renderer::CreateDebugMessenger()
  {
    #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{};
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = vkdebug::debug_utils_messenger_callback;

    VkResult result = vkdebug::CreateDebugUtilsMessenger(m_VulkanInstance, &debug_messenger_create_info, nullptr, &m_DebugMessenger);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the debug messenger");
    #endif
  }

  void Renderer::DestroyVulkanInstance()
  {
    vkDestroyInstance(m_VulkanInstance, nullptr);
  }

  void Renderer::DestroyDebugMessenger()
  {
    #if defined STATUS_DEBUG || defined STATUS_RELEASE
    vkdebug::DestroyDebugUtilsMessenger(m_VulkanInstance, m_DebugMessenger, nullptr);
    #endif
  }

}