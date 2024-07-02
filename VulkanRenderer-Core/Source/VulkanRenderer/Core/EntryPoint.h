#pragma once

#include "VulkanRenderer/Core/Application.h"

extern vkren::Application* vkren::CreateApplication();

int main()
{
  vkren::Application* app = vkren::CreateApplication();
  app->Run();
  delete app;
}