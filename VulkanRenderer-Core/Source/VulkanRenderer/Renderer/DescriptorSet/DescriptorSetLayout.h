#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"

namespace vkren
{

  struct DescriptorSetLayoutBinding
  {
    uint32_t Binding;
    VkDescriptorType Type;
    VkShaderStageFlags ShaderStage;
    uint32_t Count;
    const VkSampler* ImmutableSamplers;
  };

  typedef std::vector<DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
  class DescriptorSetLayoutConfig
  {
  public:
    void Add(uint32_t binding, const VkDescriptorType& type, const VkShaderStageFlags& stage, uint32_t count = 1, const VkSampler* immutable_samplers = VK_NULL_HANDLE);

    const DescriptorSetLayoutBindings& GetBindings() const;

  private:
    std::set<uint32_t> m_UsedBindings;
    DescriptorSetLayoutBindings m_Bindings;
  };

  class DescriptorSetLayout
  {
  public:
    ~DescriptorSetLayout();

    const VkDescriptorSetLayout& Get() const { return m_Layout; }
    const DescriptorSetLayoutBindings& GetBindings() const { return m_Bindings; }

    static Ref<DescriptorSetLayout> Create(const DescriptorSetLayoutConfig& config);

  private:
    VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
    DescriptorSetLayoutBindings m_Bindings;
  };


}