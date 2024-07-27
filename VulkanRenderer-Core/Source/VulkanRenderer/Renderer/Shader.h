#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Device.h"

namespace vkren
{

  struct DescriptorInfo
  {
    uint32_t Binding;
    VkDescriptorType Type;
    VkShaderStageFlags ShaderStage;
  };

  class DescriptorSetConfig
  {
  public:
    DescriptorSetConfig() = default;

    void AddBinding(VkDescriptorType type, VkShaderStageFlags stage);
    void AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage);

    const std::vector<DescriptorInfo>& Data() const { return m_DescriptorInfos; }

  private:
    uint32_t m_DescriptorCount = 0;
    std::set<uint32_t> m_UsedBindings;
    std::vector<DescriptorInfo> m_DescriptorInfos;
  };

  class Shader
  {
  public:
    Shader(const std::filesystem::path& vert_shader, const std::filesystem::path& frag_shader, const DescriptorSetConfig& descriptorSetConfig);
    ~Shader();

    VkShaderModule GetVertShaderModule();
    VkShaderModule GetFragShaderModule();
    const VkDescriptorSetLayout& GetDescriptorSetLayout() const;
    const std::vector<DescriptorInfo>& GetDescriptorInfos() const;

    void Destroy();

  private:
    Ref<Device> r_Device;

    std::filesystem::path m_VertShaderFilepath;
    std::vector<char> m_VertShaderCode;
    std::filesystem::path m_FragShaderFilepath;
    std::vector<char> m_FragShaderCode;

    std::vector<DescriptorInfo> m_DescriptorInfos;
    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
  };

}