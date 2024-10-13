#pragma once

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Core/Base.h"

extern vkren::Application* vkren::CreateApplication();

#if defined(PLATFORM_WINDOWS) && defined(WINDOW_APPLICATION)

#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  vkren::Log::Init();
  vkren::Application* app = vkren::CreateApplication();
  app->Run();
  delete app;
}

#else
 
int main()
{
  vkren::Log::Init();
  vkren::Application* app = vkren::CreateApplication();
  app->Run();
  delete app;
}

#endif