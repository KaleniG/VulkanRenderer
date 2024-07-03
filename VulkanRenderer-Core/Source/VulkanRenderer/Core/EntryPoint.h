#pragma once

#include "VulkanRenderer/Core/Application.h"
#include "VulkanRenderer/Core/Base.h"

extern vkren::Application* vkren::CreateApplication();

int main()
{
  vkren::Log::Init();
  vkren::Application* app = vkren::CreateApplication();
  app->Run();
  delete app;
}