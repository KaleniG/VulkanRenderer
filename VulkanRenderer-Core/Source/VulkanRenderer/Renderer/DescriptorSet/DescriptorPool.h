#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/DescriptorSet/DescriptorSetLayout.h"

namespace vkren
{

  typedef std::vector<DescriptorSetLayout> DescriptorSetLayouts;

  class DescriptorPool
  {
  public:
    ~DescriptorPool();

    const VkDescriptorPool& Get() const { return m_Pool; }

    static Ref<DescriptorPool> Create(const DescriptorSetLayouts& all_layouts, bool imgui = true);

  private:
    VkDescriptorPool m_Pool = VK_NULL_HANDLE;
  };

}