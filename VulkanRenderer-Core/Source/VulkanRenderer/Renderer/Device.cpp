#include <vkrenpch.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
  {
    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    {
      CORE_TRACE("[VULKAN] {}", callback_data->pMessage);
      break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    {
      CORE_INFO("[VULKAN] {}", callback_data->pMessage);
      break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    {
      CORE_WARN("[VULKAN] {}", callback_data->pMessage);
      break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    {
      CORE_ERROR("[VULKAN] {}", callback_data->pMessage);
      break;
    }
    }

    return VK_FALSE;
  }

  VkResult CreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger)
  {
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
      func(instance, create_info, allocator, debug_messenger);
      return VK_SUCCESS;
    }
    else
    {
      CORE_ERROR("[VULKAN] 'vkCreateDebugUtilsMessengerEXT' isn't present");
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
  {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
      func(instance, debug_messenger, allocator);
  }

  struct DeviceReqs
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

  static DeviceReqs* s_DeviceReqs = new DeviceReqs;

  Device::Device(const DeviceConfig& config)
    : m_DeviceConfig(config)
  {
    Device::CreateVulkanInstance();
    Device::CreateDebugMessenger();
    Device::CreateSurface();
    Device::ChoosePhysicalDevice();
    Device::CreateLogicalDevice();
    Device::CreateCommandSystem();
    Device::CreateRenderPass();
    Device::CreateSyncObjects();
  }

  Device::~Device()
  {
    for (int i = 0; i < m_DeviceConfig.MaxFramesInFlight; i++)
    {
      vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphores[i], VK_NULL_HANDLE);
      vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphores[i], VK_NULL_HANDLE);
      vkDestroyFence(m_LogicalDevice, m_InFlightFences[i], VK_NULL_HANDLE);
    }
    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
    vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, VK_NULL_HANDLE);
    vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, VK_NULL_HANDLE);
    vkDestroyDevice(m_LogicalDevice, VK_NULL_HANDLE);
    vkDestroySurfaceKHR(m_VulkanInstance, m_Surface, VK_NULL_HANDLE);
    #if defined STATUS_DEBUG || defined STATUS_RELEASE
    DestroyDebugUtilsMessenger(m_VulkanInstance, m_DebugMessenger, VK_NULL_HANDLE);
    #endif
    vkDestroyInstance(m_VulkanInstance, VK_NULL_HANDLE);
  }

  const VkExtent2D& Device::GetSurfaceExtent() const
  {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities);

    if (surfaceCapabilities.currentExtent.width != (uint32_t)std::numeric_limits<uint32_t>::max())
      return surfaceCapabilities.currentExtent;
    else
    {
      int32_t width, height;
      glfwGetFramebufferSize(Application::GetWindow().GetNative(), &width, &height);

      VkExtent2D actualExtent;
      actualExtent.width = static_cast<uint32_t>(width);
      actualExtent.height = static_cast<uint32_t>(height);

      actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
      actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

      return actualExtent;
    }
  }

  void Device::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
  {
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(m_LogicalDevice, &bufferCreateInfo, VK_NULL_HANDLE, &buffer);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo{};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = Device::FindMemoryType(memoryRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(m_LogicalDevice, &memoryAllocInfo, VK_NULL_HANDLE, &buffer_memory);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory for the buffer");

    result = vkBindBufferMemory(m_LogicalDevice, buffer, buffer_memory, 0);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the buffer memory");
  }

  void Device::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory)
  {
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(m_LogicalDevice, &imageCreateInfo, VK_NULL_HANDLE, &image);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(m_LogicalDevice, image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocInfo{};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memoryRequirements.size;
    memoryAllocInfo.memoryTypeIndex = Device::FindMemoryType(memoryRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(m_LogicalDevice, &memoryAllocInfo, VK_NULL_HANDLE, &image_memory);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate memory to the image");

    result = vkBindImageMemory(m_LogicalDevice, image, image_memory, 0);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to bind the image memoey");
  }

  VkImageView Device::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
  {
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.subresourceRange.aspectMask = aspect_flags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

    VkImageView imageView;
    VkResult result = vkCreateImageView(m_LogicalDevice, &imageViewCreateInfo, VK_NULL_HANDLE, &imageView);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the image view");

    return imageView;
  }

  void Device::CmdTransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
  {
    VkCommandBuffer commandBuffer = Device::BeginSingleTimeCommands();

    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = old_layout;
    imageMemoryBarrier.newLayout = new_layout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.srcAccessMask = 0; // TODO
    imageMemoryBarrier.dstAccessMask = 0; // TODO
    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
      imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
      if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
        imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
      imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkPipelineStageFlags pipelineSourceStage;
    VkPipelineStageFlags pipelineDestinationStage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      pipelineSourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      pipelineDestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      pipelineSourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      pipelineDestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

      pipelineSourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      pipelineDestinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
      CORE_ASSERT(false, "[VULKAN] Unsupported layout transition");

    vkCmdPipelineBarrier(commandBuffer, pipelineSourceStage, pipelineDestinationStage, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier);

    Device::EndSingleTimeCommands(commandBuffer);
  }

  void Device::CmdCopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
  {
    VkCommandBuffer commandBuffer = Device::BeginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    Device::EndSingleTimeCommands(commandBuffer);
  }

  void Device::CmdCopyBufferToBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
  {
    VkCommandBuffer commandBuffer = Device::BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;

    vkCmdCopyBuffer(commandBuffer, src_buffer, dst_buffer, 1, &copyRegion);

    Device::EndSingleTimeCommands(commandBuffer);
  }

  void Device::CreateVulkanInstance()
  {
    // LAYERS SUPPORT
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, VK_NULL_HANDLE);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool layersSupport = true;
    for (const char* reqiredLayer : s_DeviceReqs->Layers)
    {
      bool layerSupported = false;
      for (const VkLayerProperties& availableLayer : availableLayers)
      {
        if (std::strcmp(reqiredLayer, availableLayer.layerName) == 0)
        {
          layerSupported = true;
          break;
        }
      }

      layersSupport = layersSupport && layerSupported;

      if (layerSupported)
        continue;

      CORE_ERROR("[VULKAN] '{}' layer is not supported or not-existent", reqiredLayer);
    }
    CORE_ASSERT(layersSupport, "[VULKAN] Invalid vulkan layers support");

    // INSTANCE EXTENSION SUPPORT
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &extensionCount, availableExtensions.data());

    bool extensionsSupport = true;
    for (const char* reqiredExtension : s_DeviceReqs->InstanceExtensions)
    {
      bool extensionSupported = false;
      for (const VkExtensionProperties& availableExtension : availableExtensions)
      {
        if (std::strcmp(reqiredExtension, availableExtension.extensionName) == 0)
        {
          extensionSupported = true;
          break;
        }
      }
      extensionsSupport = extensionsSupport && extensionSupported;

      if (extensionSupported)
        continue;

      CORE_ERROR("[VULKAN] '{}' extension is not supported or not-existent", reqiredExtension);
    }
    CORE_ASSERT(extensionsSupport, "[VULKAN] Invalid vulkan extension support");

    // EVERYTHING SUPPORTED
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = Application::GetName().c_str();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pEngineName = Application::GetName().c_str();
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceReqs->InstanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = s_DeviceReqs->InstanceExtensions.data();
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(s_DeviceReqs->Layers.size());
    instanceCreateInfo.ppEnabledLayerNames = s_DeviceReqs->Layers.data();

    #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback = debug_utils_messenger_callback;
    debugUtilsMessengerCreateInfo.pUserData = VK_NULL_HANDLE;
    instanceCreateInfo.pNext = &debugUtilsMessengerCreateInfo;
    #else
    create_info.pNext = VK_NULL_HANDLE;
    #endif

    VkResult result = vkCreateInstance(&instanceCreateInfo, VK_NULL_HANDLE, &m_VulkanInstance);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the Vulkan instance");
  }

  void Device::CreateDebugMessenger()
  {
    #if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback = debug_utils_messenger_callback;
    debugUtilsMessengerCreateInfo.pUserData = VK_NULL_HANDLE;

    VkResult result = CreateDebugUtilsMessenger(m_VulkanInstance, &debugUtilsMessengerCreateInfo, VK_NULL_HANDLE, &m_DebugMessenger);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the debug messenger");
    #endif
  }

  void Device::CreateSurface()
  {
    VkResult result = glfwCreateWindowSurface(m_VulkanInstance, Application::GetWindow().GetNative(), VK_NULL_HANDLE, &m_Surface);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN/GLFW] Failed to create a surface");
  }

  void Device::ChoosePhysicalDevice()
  {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, VK_NULL_HANDLE);
    CORE_ASSERT(deviceCount != 0, "[VULKAN] Failed to find a physical device");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.data());

    #if defined STATUS_DEBUG || defined STATUS_RELEASE
    std::string physicalDevicesStr;
    for (uint32_t i = 0; i < deviceCount; i++)
    {
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
      physicalDevicesStr.append(" (");
      physicalDevicesStr.append(deviceProperties.deviceName);
      physicalDevicesStr.append(")");
    }
    CORE_TRACE("[VULKAN] Found {} physical devices: {}", deviceCount, physicalDevicesStr);
    #endif

    for (const VkPhysicalDevice& device : devices)
    {
      // QUEUE SUPPORT
      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, VK_NULL_HANDLE);
      std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

      std::optional<uint32_t> graphicsQueueIndex;
      std::optional<uint32_t> presentQueueIndex;

      int i = 0;
      for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
      {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
          graphicsQueueIndex = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

        if (presentSupport)
          presentQueueIndex = i;

        if (graphicsQueueIndex.has_value() && presentQueueIndex.has_value())
          break;

        i++;
      }

      bool queueSupport = graphicsQueueIndex.has_value() && presentQueueIndex.has_value();
      if (!queueSupport)
        continue;

      // FEATURE SUPPORT
      VkPhysicalDeviceFeatures supportedFeatures;
      vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
      bool feature_support = supportedFeatures.samplerAnisotropy;
      if (!feature_support)
        continue;

      // SWAPCHAIN SUPPORT
      std::vector<VkSurfaceFormatKHR> surfaceFormats;
      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, VK_NULL_HANDLE);
      surfaceFormats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, surfaceFormats.data());

      std::vector<VkPresentModeKHR> presentModes;
      uint32_t presentModeCount;
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, VK_NULL_HANDLE);
      presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, presentModes.data());

      bool swapchainSupport = !surfaceFormats.empty() && !presentModes.empty();
      if (!swapchainSupport)
        continue;

      // DEVICE EXTENSION SUPPORT
      uint32_t extensionCount;
      vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE);
      std::vector<VkExtensionProperties> availableExtensions(extensionCount);
      vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extensionCount, availableExtensions.data());

      bool extensionSupport = true;
      for (const char* reqiredExtension : s_DeviceReqs->DeviceExtensions)
      {
        bool extensionSupported = false;
        for (const VkExtensionProperties& availableExtension : availableExtensions)
        {
          if (std::strcmp(reqiredExtension, availableExtension.extensionName) == 0)
          {
            extensionSupported = true;
            break;
          }
        }

        extensionSupport = extensionSupport && extensionSupported;

        if (extensionSupported)
          continue;

        #if defined STATUS_DEBUG || defined STATUS_RELEASE
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        CORE_WARN("[VULKAN] '{}' is not-existent or isn't supported by {} physical device", reqiredExtension, device_properties.deviceName);
        #endif
      }

      if (!extensionSupport)
        continue;

      // EVERYTHING SUPPORTED
      m_PhysicalDevice = device;
      m_GraphicsQueueFamilyIndex = graphicsQueueIndex.value();
      m_PresentQueueFamilyIndex = presentQueueIndex.value();
      break;
    }

    CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "[VULKAN] Failed to find a suitable physical device");
  }

  void Device::CreateLogicalDevice()
  {
    std::set<uint32_t> uniqueQueueFamilies = { m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex };
    std::vector<VkDeviceQueueCreateInfo> logicalDeviceQueueCreateInfos;

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
      VkDeviceQueueCreateInfo logicalDeviceQueueCreateInfo{};
      logicalDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      logicalDeviceQueueCreateInfo.queueFamilyIndex = queueFamily;
      logicalDeviceQueueCreateInfo.queueCount = 1;
      logicalDeviceQueueCreateInfo.pQueuePriorities = &queuePriority;
      logicalDeviceQueueCreateInfos.push_back(logicalDeviceQueueCreateInfo);
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures{};
    physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo logicalDeviceCreateInfo{};
    logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(logicalDeviceQueueCreateInfos.size());
    logicalDeviceCreateInfo.pQueueCreateInfos = logicalDeviceQueueCreateInfos.data();
    logicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceReqs->DeviceExtensions.size());
    logicalDeviceCreateInfo.ppEnabledExtensionNames = s_DeviceReqs->DeviceExtensions.data();
    logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(s_DeviceReqs->Layers.size());
    logicalDeviceCreateInfo.ppEnabledLayerNames = s_DeviceReqs->Layers.data();

    VkResult result = vkCreateDevice(m_PhysicalDevice, &logicalDeviceCreateInfo, VK_NULL_HANDLE, &m_LogicalDevice);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the logical device");

    vkGetDeviceQueue(m_LogicalDevice, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_LogicalDevice, m_PresentQueueFamilyIndex, 0, &m_PresentQueue);

    delete s_DeviceReqs;
  }

  void Device::CreateCommandSystem()
  {
    // COMMAND POOL
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;

    VkResult result = vkCreateCommandPool(m_LogicalDevice, &commandPoolCreateInfo, VK_NULL_HANDLE, &m_CommandPool);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the command pool");

    // COMMAND BUFFERS
    m_CommandBuffers.resize(m_DeviceConfig.MaxFramesInFlight);

    VkCommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.commandPool = m_CommandPool;
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    result = vkAllocateCommandBuffers(m_LogicalDevice, &commandBufferAllocInfo, m_CommandBuffers.data());
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the command buffers");
  }

  void Device::CreateRenderPass()
  {
    std::array<VkFormat, 3> depthFormats =
    {
      VK_FORMAT_D32_SFLOAT,
      VK_FORMAT_D32_SFLOAT_S8_UINT,
      VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : depthFormats)
    {
      VkFormatProperties formatProperties;
      vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &formatProperties);

      if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        m_DepthAttachmentFormat = format;
    }

    //CORE_ASSERT(, "[VULKAN] Failed to find a supported depth attachment format");

    // SURFACE IMAGE FORMAT
    uint32_t surfaceImageFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceImageFormatCount, VK_NULL_HANDLE);
    std::vector<VkSurfaceFormatKHR> surfaceImageFormats(surfaceImageFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceImageFormatCount, surfaceImageFormats.data());
    VkSurfaceFormatKHR m_SurfaceFormat = surfaceImageFormats[0];
    for (const VkSurfaceFormatKHR& surfaceFormat : surfaceImageFormats)
    {
      if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      {
        m_SurfaceFormat = surfaceFormat;
        break;
      }
    }

    // COLOR ATTACHMENT
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_SurfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // DEPTH ATTACHMENT
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = m_DepthAttachmentFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference{};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // SUBPASS
    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = VK_NULL_HANDLE;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = VK_NULL_HANDLE;
    subpassDescription.pResolveAttachments = VK_NULL_HANDLE;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderpassCreateInfo{};
    renderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderpassCreateInfo.pAttachments = attachments.data();
    renderpassCreateInfo.subpassCount = 1;
    renderpassCreateInfo.pSubpasses = &subpassDescription;
    renderpassCreateInfo.dependencyCount = 1;
    renderpassCreateInfo.pDependencies = &subpassDependency;

    VkResult result = vkCreateRenderPass(m_LogicalDevice, &renderpassCreateInfo, VK_NULL_HANDLE, &m_RenderPass);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the renderpass");
  }

  void Device::CreateSyncObjects()
  {
    m_ImageAvailableSemaphores.resize(m_DeviceConfig.MaxFramesInFlight);
    m_RenderFinishedSemaphores.resize(m_DeviceConfig.MaxFramesInFlight);
    m_InFlightFences.resize(m_DeviceConfig.MaxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result;

    for (size_t i = 0; i < m_DeviceConfig.MaxFramesInFlight; i++)
    {
      result = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, VK_NULL_HANDLE, &m_ImageAvailableSemaphores[i]);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create an 'ImageAvailable' semaphore");

      result = vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, VK_NULL_HANDLE, &m_RenderFinishedSemaphores[i]);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create an 'RenderFinished' semaphore");

      result = vkCreateFence(m_LogicalDevice, &fenceInfo, nullptr, &m_InFlightFences[i]);
      CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create an 'InFlight' fence");
    }
  }

  uint32_t Device::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
  {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

    for (int i = 0; i < memoryProperties.memoryTypeCount; i++)
      if (type_filter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        return i;

    CORE_ASSERT(false, "[VULKAN] Failed to find a required memory type");
  }

  VkCommandBuffer Device::BeginSingleTimeCommands()
  {
    VkCommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandPool = m_CommandPool;
    commandBufferAllocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_LogicalDevice, &commandBufferAllocInfo, &commandBuffer);

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    return commandBuffer;
  }

  void Device::EndSingleTimeCommands(VkCommandBuffer command_buffer)
  {
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer;

    vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_GraphicsQueue);

    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &command_buffer);
  }

}