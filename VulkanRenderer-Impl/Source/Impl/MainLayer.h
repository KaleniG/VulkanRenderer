#pragma once

#include <VulkanRenderer.h>

namespace vkren
{
  class MainLayer : public Layer
  {
  public:
    MainLayer() : Layer("Applayer") {}
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
}