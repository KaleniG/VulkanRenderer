#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Device.h"
#include "VulkanRenderer/Renderer/Shader.h"

namespace vkren
{
  class GraphicsPipeline
  {
  public:
    GraphicsPipeline(Device& device, const Ref<Shader>& shader);

    const VkPipelineLayout& GetLayout() const;
    const VkPipeline& Get() const;

    void Destroy();
  private:
    Device& r_Device;

    VkPipelineLayout m_Layout = VK_NULL_HANDLE;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
  };
}