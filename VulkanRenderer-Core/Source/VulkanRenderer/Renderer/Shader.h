#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  class DescriptorSetBindings
  {
  public:
    void AddBinding(VkDescriptorType type, VkShaderStageFlags stage);
    void AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage);

    const uint32_t GetCount() const { return m_Bindings.size(); }
    const VkDescriptorSetLayoutBinding* GetData() const { return m_Bindings.data(); }

  private:
    uint32_t m_BindingCount = 0;
    std::set<uint32_t> m_UsedBindings;
    std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
  };

  class Shader
  {
  public:
    Shader(Device& device, const std::filesystem::path& vert_shader, const std::filesystem::path& frag_shader, const DescriptorSetBindings& bindings);

    VkShaderModule GetVertShaderModule();
    VkShaderModule GetFragShaderModule();
    const VkDescriptorSetLayout& GetDescriptorSetLayout() const;

    void Destroy();

  private:
    Device& r_Device;

    std::filesystem::path m_VertShaderFilepath;
    std::vector<char> m_VertShaderCode;
    std::filesystem::path m_FragShaderFilepath;
    std::vector<char> m_FragShaderCode;

    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
  };

}