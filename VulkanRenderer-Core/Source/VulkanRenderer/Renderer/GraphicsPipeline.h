#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Device.h"
#include "VulkanRenderer/Renderer/Shader.h"

namespace vkren
{

  class GraphicsPipeline
  {
  public:
    GraphicsPipeline(Ref<Shader> shader);
    ~GraphicsPipeline();

    const VkPipelineLayout& GetLayout() const; // MODIFY
    const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }
    const VkPipeline& Get() const; // MODIFY

    void Destroy(); // MODIFY

  private:
    void CreatePipeline();
    void CreateDescriptorPool();
    void CreateDescriptorSets();

  private:
    Ref<Device> r_Device;
    Ref<Shader> r_Shader;

    VkPipelineLayout m_Layout = VK_NULL_HANDLE;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;

    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_DescriptorSets;
  };
}