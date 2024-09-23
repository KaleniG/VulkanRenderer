#pragma once

#include <vulkan/vulkan.h>

namespace vkren
{

  class RenderPass
  {
  public:
    RenderPass();

  private:
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
  };

}