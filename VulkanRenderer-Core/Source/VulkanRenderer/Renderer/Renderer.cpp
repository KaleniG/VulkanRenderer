#include <vkrenpch.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/DebugMessenger.h"

namespace vkren
{

  struct RendererRequirements
  {
    std::array<const char*, 1> Layers =
    {
      #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
      "VK_LAYER_KHRONOS_validation"
      #endif
    };

    std::array<const char*, 3> InstanceExtensions =
    {
      "VK_KHR_surface",
      "VK_KHR_win32_surface",
      #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
      "VK_EXT_debug_utils"
      #endif
    };

    std::array<const char*, 1> DeviceExtensions =
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
    Renderer::CreateSurface();
    Renderer::ChoosePhysicalDevice();
  }

  void Renderer::Shutdown()
  {
    Renderer::DestroyLogicalDevice();
    Renderer::DestroySurface();
    Renderer::DestroyDebugMessenger();
    Renderer::DestroyVulkanInstance();
  }

  void Renderer::CreateVulkanInstance()
  {
    // LAYERS SUPPORT
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    bool layers_support = true;
    for (const char* reqired_layer : s_RendererRequirements->Layers)
    {
      bool layer_supported = false;
      for (const VkLayerProperties& available_layer : available_layers)
      {
        if (std::strcmp(reqired_layer, available_layer.layerName) == 0)
        {
          layer_supported = true;
          break;
        }
      }

      layers_support = layers_support && layer_supported;

      if (layer_supported)
        continue;

      CORE_ERROR("[VULKAN] '{}' layer is not supported or not-existent", reqired_layer);
    }
    CORE_ASSERT(layers_support, "[VULKAN] Invalid vulkan layers support");

    // INSTANCE EXTENSION SUPPORT
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data());

    bool extensions_support = true;
    for (const char* reqired_extension : s_RendererRequirements->InstanceExtensions)
    {
      bool extension_supported = false;
      for (const VkExtensionProperties& available_extension : available_extensions)
      {
        if (std::strcmp(reqired_extension, available_extension.extensionName) == 0)
        {
          extension_supported = true;
          break;
        }
      }
      extensions_support = extensions_support && extension_supported;

      if (extension_supported)
        continue;

      CORE_ERROR("[VULKAN] '{}' extension is not supported or not-existent", reqired_extension);
    }
    CORE_ASSERT(extensions_support, "[VULKAN] Invalid vulkan extension support");

    // EVERYTHING SUPPORTED
    VkApplicationInfo app_info;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "VulkanTutorial";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "None";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pNext = nullptr;

    VkInstanceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.flags = 0;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(s_RendererRequirements->InstanceExtensions.size());
    create_info.ppEnabledExtensionNames = s_RendererRequirements->InstanceExtensions.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(s_RendererRequirements->Layers.size());
    create_info.ppEnabledLayerNames = s_RendererRequirements->Layers.data();

    #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info;
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.flags = 0;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = vkdebug::debug_utils_messenger_callback;
    debug_messenger_create_info.pUserData = nullptr;
    debug_messenger_create_info.pNext = nullptr;
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_messenger_create_info;
    #else
    create_info.pNext = nullptr;
    #endif

    VkResult result = vkCreateInstance(&create_info, nullptr, &m_VulkanInstance);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to instantiate Vulkan");
  }

  void Renderer::CreateDebugMessenger()
  {
    #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info;
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.flags = 0;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = vkdebug::debug_utils_messenger_callback;
    debug_messenger_create_info.pUserData = nullptr;
    debug_messenger_create_info.pNext = nullptr;

    VkResult result = vkdebug::CreateDebugUtilsMessenger(m_VulkanInstance, &debug_messenger_create_info, nullptr, &m_DebugMessenger);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the debug messenger");
    #endif
  }

  void Renderer::CreateSurface()
  {
    VkResult result = glfwCreateWindowSurface(m_VulkanInstance, m_WindowReference->GetNative(), nullptr, &m_Surface);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN/GLFW] Failed to create a surface");
  }

  void Renderer::ChoosePhysicalDevice()
  {
    m_PhysicalDevice = VK_NULL_HANDLE;

    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_VulkanInstance, &device_count, nullptr);
    CORE_ASSERT(device_count != 0, "[VULKAN] Failed to find a physical device");

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_VulkanInstance, &device_count, devices.data());

    #if defined STATUS_DEBUG || defined STATUS_RELEASE
    std::string physical_devices_str;
    for (uint32_t i = 0; i < device_count; i++)
    {
      VkPhysicalDeviceProperties device_properties;
      vkGetPhysicalDeviceProperties(devices[i], &device_properties);
      physical_devices_str.append(" (");
      physical_devices_str.append(device_properties.deviceName);
      physical_devices_str.append(")");
    }
    CORE_TRACE("[VULKAN] Found {} physical devices: {}", device_count, physical_devices_str);
    #endif

    for (const VkPhysicalDevice& device : devices)
    {
      // QUEUE SUPPORT
      uint32_t queue_family_count = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
      std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
      
      std::optional<uint32_t> graphics_queue_index;
      std::optional<uint32_t> present_queue_index;

      int i = 0;
      for (const auto& queue_family : queue_families)
      {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
          graphics_queue_index = i;

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &present_support);

        if (present_support)
          present_queue_index = i;

        if (graphics_queue_index.has_value() && present_queue_index.has_value())
          break;

        i++;
      }

      bool queue_support = graphics_queue_index.has_value() && present_queue_index.has_value();
      if (!queue_support)
        continue;

      // FEATURE SUPPORT
      VkPhysicalDeviceFeatures supported_features;
      vkGetPhysicalDeviceFeatures(device, &supported_features);
      bool feature_support = supported_features.samplerAnisotropy;
      if (!feature_support)
        continue;

      // SWAPCHAIN SUPPORT
      std::vector<VkSurfaceFormatKHR> surface_formats;
      uint32_t format_count;
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &format_count, nullptr);
      surface_formats.resize(format_count);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &format_count, surface_formats.data());

      std::vector<VkPresentModeKHR> present_modes;
      uint32_t present_mode_count;
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &present_mode_count, nullptr);
      present_modes.resize(present_mode_count);
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &present_mode_count, present_modes.data());

      bool swapchain_support = !surface_formats.empty() && !present_modes.empty();
      if (!swapchain_support)
        continue;

      // DEVICE EXTENSION SUPPORT
      uint32_t extension_count;
      vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
      std::vector<VkExtensionProperties> available_extensions(extension_count);
      vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

      bool extension_support = true;
      for (const char* reqired_extension : s_RendererRequirements->DeviceExtensions)
      {
        bool extension_supported = false;
        for (const VkExtensionProperties& available_extension : available_extensions)
        {
          if (std::strcmp(reqired_extension, available_extension.extensionName) == 0)
          {
            extension_supported = true;
            break;
          }
        }

        extension_support = extension_support && extension_supported;

        if (extension_supported)
          continue;

        #if defined STATUS_DEBUG || defined STATUS_RELEASE
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        #endif

        CORE_WARN("[VULKAN] '{}' is not-existent or isn't supported by {} physical device", reqired_extension, device_properties.deviceName);
      }

      if (!extension_support)
        continue;

      // EVERYTHING SUPPORTED
      m_PhysicalDevice = device;
      m_GraphicsQueueInfo.FamilyIndex = graphics_queue_index.value();
      m_PresentQueueInfo.FamilyIndex = present_queue_index.value();
      break;
    }

    CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "[VULKAN] Failed to find a suitable physical device");
  }

  void Renderer::CreateLogicalDevice()
  {
    std::set<uint32_t> unique_queue_families = { m_GraphicsQueueInfo.FamilyIndex, m_PresentQueueInfo.FamilyIndex };
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families) 
    {
      VkDeviceQueueCreateInfo queue_create_info;
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.flags = 0;
      queue_create_info.queueFamilyIndex = queue_family;
      queue_create_info.queueCount = 1;
      queue_create_info.pQueuePriorities = &queue_priority;
      queue_create_info.pNext = nullptr;
      queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures features;
    features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo logical_device_create_info;
    logical_device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logical_device_create_info.flags = 0;
    logical_device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    logical_device_create_info.pQueueCreateInfos = queue_create_infos.data();
    logical_device_create_info.pEnabledFeatures = &features;
    logical_device_create_info.enabledExtensionCount = static_cast<uint32_t>(s_RendererRequirements->DeviceExtensions.size());
    logical_device_create_info.ppEnabledExtensionNames = s_RendererRequirements->DeviceExtensions.data();
    logical_device_create_info.enabledLayerCount = static_cast<uint32_t>(s_RendererRequirements->Layers.size());
    logical_device_create_info.ppEnabledLayerNames = s_RendererRequirements->Layers.data();
    logical_device_create_info.pNext = nullptr;

    VkResult result = vkCreateDevice(m_PhysicalDevice, &logical_device_create_info, nullptr, &m_LogicalDevice);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the logical device");

    vkGetDeviceQueue(m_LogicalDevice, m_GraphicsQueueInfo.FamilyIndex, 0, &m_GraphicsQueueInfo.Queue);
    vkGetDeviceQueue(m_LogicalDevice, m_PresentQueueInfo.FamilyIndex, 0, &m_PresentQueueInfo.Queue);

    delete s_RendererRequirements; // Because it is not needed anymore (Mayebe it is a bad idea)
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

  void Renderer::DestroySurface()
  {
    vkDestroySurfaceKHR(m_VulkanInstance, m_Surface, nullptr);
  }

  void Renderer::DestroyLogicalDevice()
  {
    vkDestroyDevice(m_LogicalDevice, nullptr);
  }

}