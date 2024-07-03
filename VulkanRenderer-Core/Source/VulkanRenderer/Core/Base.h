#pragma once

#include <filesystem>

#include "VulkanRenderer/Core/Log.h"

#if defined(STATUS_DEBUG) || defined(STATUS_RELEASE)
  #define VKREN_EXPAND_MACRO(x) x
  #define VKREN_STRINGIFY_MACRO(x) #x
  #define VKREN_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { ##type##ERROR(msg, __VA_ARGS__); __debugbreak(); } }
  #define VKREN_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VKREN_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
  #define VKREN_INTERNAL_ASSERT_NO_MSG(type, check) VKREN_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", VKREN_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
  #define VKREN_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
  #define VKREN_INTERNAL_ASSERT_GET_MACRO(...) VKREN_EXPAND_MACRO( VKREN_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VKREN_INTERNAL_ASSERT_WITH_MSG, VKREN_INTERNAL_ASSERT_NO_MSG) )
  
  #define CORE_INFO(...)  ::vkren::Log::GetCoreLogger()->info(__VA_ARGS__)
  #define CORE_TRACE(...) ::vkren::Log::GetCoreLogger()->trace(__VA_ARGS__)
  #define CORE_WARN(...)  ::vkren::Log::GetCoreLogger()->warn(__VA_ARGS__)
  #define CORE_ERROR(...) ::vkren::Log::GetCoreLogger()->error(__VA_ARGS__)
  #define CORE_FATAL(...) ::vkren::Log::GetCoreLogger()->critical(__VA_ARGS__)
  #define CORE_ASSERT(...) VKREN_EXPAND_MACRO( VKREN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(CORE_, __VA_ARGS__) )
  
  #define IMPL_INFO(...)  ::vkren::Log::GetImplLogger()->info(__VA_ARGS__)
  #define IMPL_TRACE(...) ::vkren::Log::GetImplLogger()->trace(__VA_ARGS__)
  #define IMPL_WARN(...)  ::vkren::Log::GetImplLogger()->warn(__VA_ARGS__)
  #define IMPL_ERROR(...) ::vkren::Log::GetImplLogger()->error(__VA_ARGS__)
  #define IMPL_FATAL(...) ::vkren::Log::GetImplLogger()->critical(__VA_ARGS__)
  #define IMPL_ASSERT(...) VKREN_EXPAND_MACRO( VKREN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(IMPL_, __VA_ARGS__) )
#else
  #define CORE_INFO(...)
  #define CORE_TRACE(...)
  #define CORE_WARN(...)
  #define CORE_ERROR(...)
  #define CORE_FATAL(...)
  #define CORE_ASSERT(...)
  
  #define IMPL_INFO(...)
  #define IMPL_TRACE(...)
  #define IMPL_WARN(...)
  #define IMPL_ERROR(...)
  #define IMPL_FATAL(...)
  #define IMPL_ASSERT(...)
#endif