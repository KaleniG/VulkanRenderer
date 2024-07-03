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
  
IncludeDir = {}
IncludeDir["spdlog"] = "VulkanRenderer-Core/Vendor/spdlog/include"

project "VulkanRenderer-Core"
  location "VulkanRenderer-Core"
  kind "StaticLib"
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
    "%{prj.name}/Source",
    "%{IncludeDir.spdlog}"
  }

  links
  {

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
    "%{prj.name}/Source",
    "VulkanRenderer-Core/Source",
    "%{IncludeDir.spdlog}"
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