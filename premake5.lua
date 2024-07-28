workspace "VulkanRenderer"
  architecture "x86_64"
  startproject "VulkanRenderer-Impl"

  configurations
  {
    "Debug",
    "Release",
    "Distribution"
  }
  
  flags
  {
    "MultiProcessorCompile"
  }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"]          = "VulkanRenderer-Core/Vendor/spdlog/include"
IncludeDir["glfw"]            = "VulkanRenderer-Core/Vendor/glfw/include"
IncludeDir["glm"]             = "VulkanRenderer-Core/Vendor/glm"
IncludeDir["VulkanSDK"]       = "%{VULKAN_SDK}/Include"
IncludeDir["stb"]             = "VulkanRenderer-Core/Vendor/stb/include"
IncludeDir["tiny_obj_loader"] = "VulkanRenderer-Core/Vendor/tiny_obj_loader"

LibDir = {}
LibDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

group "Dependencies"
  include "VulkanRenderer-Core/Vendor/glfw"
group ""

project "VulkanRenderer-Core"
  location "VulkanRenderer-Core"
  kind "StaticLib"
  language "C++"
  cppdialect "C++latest"
  staticruntime "off"

  targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
  objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")
  
  pchheader "vkrenpch.h"
  pchsource "VulkanRenderer-Core/Source/vkrenpch.cpp"

  files
  {
    "%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
    "%{prj.name}/Vendor/glm/**.h",
    "%{prj.name}/Vendor/glm/**.hpp",
		"%{prj.name}/Vendor/glm/**.inl",
    "%{prj.name}/Vendor/stb/**.h",
    "%{prj.name}/Vendor/stb/**.cpp",
    "%{prj.name}/Vendor/tiny_obj_loader/**.h",
    "%{prj.name}/Vendor/tiny_obj_loader/**.cpp"
  }

  includedirs
  {
    "%{prj.name}/Source",
    "%{IncludeDir.spdlog}",
    "%{IncludeDir.glfw}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.VulkanSDK}",
    "%{IncludeDir.stb}",
    "%{IncludeDir.tiny_obj_loader}"
  }

  libdirs
  {
    "%{LibDir.VulkanSDK}"
  }

  links
  {
    "glfw",
    "vulkan-1.lib"
  }

  filter "system:windows"
    systemversion "latest"

    defines
    {
      GLFW_INCLUDE_NONE
    }

  filter "configurations:Debug"
    runtime "Debug"
    defines "STATUS_DEBUG"
    symbols "on"

  filter "configurations:Release"
    runtime "Release"
    defines "STATUS_RELEASE"
    optimize "on"

  filter "configurations:Distribution"
    runtime "Release"
    defines "STATUS_DISTRIBUITION"
    optimize "on"

project "VulkanRenderer-Impl"
  location "VulkanRenderer-Impl"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++latest"
  staticruntime "off"

  targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
  objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

  files
  {
    "%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
  }

  includedirs
  {
    "%{prj.name}",
    "%{prj.name}/Source",
    "VulkanRenderer-Core/Source",
    "VulkanRenderer-Core/Vendor",
    "%{IncludeDir.spdlog}",
    "%{IncludeDir.glfw}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.VulkanSDK}",
    "%{IncludeDir.stb}",
    "%{IncludeDir.tiny_obj_loader}"
  }


  links
  {
    "VulkanRenderer-Core"
  }

  filter "system:windows"
    systemversion "latest"

  filter "configurations:Debug"
    runtime "Debug"
    defines "STATUS_DEBUG"
    symbols "on"

  filter "configurations:Release"
    runtime "Release"
    defines "STATUS_RELEASE"
    optimize "on"

  filter "configurations:Distribution"
    runtime "Release"
    defines "STATUS_DISTRIBUITION"
    optimize "on"