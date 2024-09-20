#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/DescriptorSet/DescriptorSetLayout.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  void DescriptorSetLayoutConfig::Add(uint32_t binding, const VkDescriptorType& type, const VkShaderStageFlags& stage, uint32_t count, const VkSampler* immutable_samplers)
  {
    CORE_ASSERT(!m_UsedBindings.contains(binding), "[SYSTEM] This binding is not available");

    m_UsedBindings.insert(binding);
    m_Bindings.push_back({binding, type, stage, count, immutable_samplers});
  }

  DescriptorSetLayoutBindings DescriptorSetLayoutConfig::GetBidings() const
  {
    CORE_ASSERT(!m_Bindings.empty(), "[SYSTEM] Cannot get an empty layout");
    return m_Bindings;
  }

  DescriptorSetLayout::~DescriptorSetLayout()
  {
    vkDestroyDescriptorSetLayout(Renderer::GetDevice().GetLogical(), m_Layout, VK_NULL_HANDLE);
  }

  Ref<DescriptorSetLayout> DescriptorSetLayout::Create(const DescriptorSetLayoutConfig& config)
  {
    Ref<DescriptorSetLayout> layout = CreateRef<DescriptorSetLayout>();
    layout->m_Bindings = config.GetBidings();

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(config.GetBidings().size());
    for (size_t i = 0; i < config.GetBidings().size(); i++)
    {
      descriptorSetLayoutBindings[i].binding = config.GetBidings()[i].Binding;
      descriptorSetLayoutBindings[i].descriptorCount = config.GetBidings()[i].Count;
      descriptorSetLayoutBindings[i].descriptorType = config.GetBidings()[i].Type;
      descriptorSetLayoutBindings[i].pImmutableSamplers = config.GetBidings()[i].ImmutableSamplers;
      descriptorSetLayoutBindings[i].stageFlags = config.GetBidings()[i].ShaderStage;
    }

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    createInfo.pBindings = descriptorSetLayoutBindings.data();

    VkResult result = vkCreateDescriptorSetLayout(Renderer::GetDevice().GetLogical(), &createInfo, VK_NULL_HANDLE, &layout->m_Layout);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the shaders' descriptor set layout. Error: {}", Utils::VkResultToString(result));

    return layout;
  }

}