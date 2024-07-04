#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"

namespace vkren
{

  namespace vkdebug
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
  }

}