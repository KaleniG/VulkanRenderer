#pragma once

#include <VulkanRenderer.h>

class MainLayer : public vkren::Layer
{
public:
  MainLayer() : vkren::Layer("Applayer") {}
  ~MainLayer() override {}

  void OnAttach() override
  {

  }

  void OnDetach() override
  {

  }

  void OnUpdate(vkren::Timestep timestep) override
  {

  }
};