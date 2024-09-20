#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Shader.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  void DescriptorSetConfig::AddBinding(VkDescriptorType type, VkShaderStageFlags stage, const Ref<Texture>& texture)
  {
    while (m_UsedBindings.contains(m_DescriptorCount))
      m_DescriptorCount++;

    m_UsedBindings.insert(m_DescriptorCount);
    m_DescriptorInfos.push_back(DescriptorInfo(m_DescriptorCount, type, stage, texture));
  }

  void DescriptorSetConfig::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, const Ref<Texture>& texture)
  {
    CORE_ASSERT(!m_UsedBindings.contains(binding), "[SYSTEM] This binding is not available");

    m_UsedBindings.insert(binding);
    m_DescriptorInfos.push_back(DescriptorInfo(binding, type, stage, texture));
  }

  void DescriptorSetConfig::AddBinding(VkDescriptorType type, VkShaderStageFlags stage, const std::vector<Ref<MUniformBuffer>>& buffers)
  {
    while (m_UsedBindings.contains(m_DescriptorCount))
      m_DescriptorCount++;

    m_UsedBindings.insert(m_DescriptorCount);
    m_DescriptorInfos.push_back(DescriptorInfo(m_DescriptorCount, type, stage, nullptr, buffers));
  }

  void DescriptorSetConfig::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, const std::vector<Ref<MUniformBuffer>>& buffers)
  {
    CORE_ASSERT(!m_UsedBindings.contains(binding), "[SYSTEM] This binding is not available");

    m_UsedBindings.insert(binding);
    m_DescriptorInfos.push_back(DescriptorInfo(binding, type, stage, nullptr, buffers));
  }

  Shader::Shader(const std::filesystem::path& vert_shader, const std::filesystem::path& frag_shader, const DescriptorSetConfig& descriptorSetConfig)
    : r_Device(Renderer::GetDeviceRef()), m_VertShaderFilepath(vert_shader), m_FragShaderFilepath(frag_shader), m_DescriptorInfos(descriptorSetConfig.Data())
  {
    Device& device = *r_Device.get();

    std::ifstream vertShaderFile(vert_shader.string(), std::ios::ate | std::ios::binary);
    CORE_ASSERT(vertShaderFile.is_open(), "[STD] Couldn't open the vertex shader file: {}", vert_shader.string());
    size_t vertShaderFileSize = static_cast<size_t>(vertShaderFile.tellg());
    m_VertShaderCode.resize(vertShaderFileSize);
    vertShaderFile.seekg(0);
    vertShaderFile.read(m_VertShaderCode.data(), vertShaderFileSize);
    vertShaderFile.close();

    std::ifstream fragShaderFile(frag_shader.string(), std::ios::ate | std::ios::binary);
    CORE_ASSERT(fragShaderFile.is_open(), "[STD] Couldn't open the fragment shader file: {}", frag_shader.string());
    size_t fragShaderFileSize = static_cast<size_t>(fragShaderFile.tellg());
    m_FragShaderCode.resize(fragShaderFileSize);
    fragShaderFile.seekg(0);
    fragShaderFile.read(m_FragShaderCode.data(), fragShaderFileSize);
    fragShaderFile.close();

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(m_DescriptorInfos.size());
    for (size_t i = 0; i < m_DescriptorInfos.size(); i++)
    {
      VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
      descriptorSetLayoutBinding.binding = m_DescriptorInfos[i].Binding;
      descriptorSetLayoutBinding.descriptorCount = 1;
      descriptorSetLayoutBinding.descriptorType = m_DescriptorInfos[i].Type;
      descriptorSetLayoutBinding.stageFlags = m_DescriptorInfos[i].ShaderStage;
      descriptorSetLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;

      descriptorSetLayoutBindings[i] = descriptorSetLayoutBinding;
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

    VkResult result = vkCreateDescriptorSetLayout(device.GetLogical(), &descriptorSetLayoutCreateInfo, VK_NULL_HANDLE, &m_DescriptorSetLayout);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the shaders' descriptor set layout. Error: {}", Utils::VkResultToString(result));
  }

  Shader::~Shader()
  {
    Shader::Destroy();
  }

  VkShaderModule Shader::GetVertShaderModule()
  {
    CORE_ASSERT(!m_VertShaderCode.empty(), "[VULKAN/SYSTEM] Vertex shader code is empty");

    Device& device = *r_Device.get();

    VkShaderModuleCreateInfo vertShaderModuleCreateInfo{};
    vertShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertShaderModuleCreateInfo.codeSize = m_VertShaderCode.size();
    vertShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(m_VertShaderCode.data());

    VkShaderModule vertShaderModule;
    VkResult result = vkCreateShaderModule(device.GetLogical(), &vertShaderModuleCreateInfo, VK_NULL_HANDLE, &vertShaderModule);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the vertex shader module. Error: {}", Utils::VkResultToString(result));

    return vertShaderModule;
  }

  VkShaderModule Shader::GetFragShaderModule()
  {
    CORE_ASSERT(!m_FragShaderCode.empty(), "[VULKAN/SYSTEM] Fragment shader code is empty");

    Device& device = *r_Device.get();

    VkShaderModuleCreateInfo fragShaderModuleCreateInfo{};
    fragShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragShaderModuleCreateInfo.codeSize = m_FragShaderCode.size();
    fragShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(m_FragShaderCode.data());

    VkShaderModule fragShaderModule;
    VkResult result = vkCreateShaderModule(device.GetLogical(), &fragShaderModuleCreateInfo, VK_NULL_HANDLE, &fragShaderModule);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the fragment shader module. Error: {}", Utils::VkResultToString(result));

    return fragShaderModule;
  }

  const VkDescriptorSetLayout& Shader::GetDescriptorSetLayout() const
  {
    CORE_ASSERT(m_DescriptorSetLayout != VK_NULL_HANDLE, "[VULKAN/SYSTEM] Descriptor set layout is invalid");
    return m_DescriptorSetLayout;
  }

  const std::vector<DescriptorInfo>& Shader::GetDescriptorInfos() const
  {
    CORE_ASSERT(!m_DescriptorInfos.empty(), "[VULKAN/SYSTEM] Descriptor infos are empty");
    return m_DescriptorInfos;
  }

  void Shader::Destroy()
  {
    CORE_ASSERT(m_DescriptorSetLayout != VK_NULL_HANDLE, "[VULKAN/SYSTEM] Descriptor set layout is invalid");

    Device& device = *r_Device.get();

    vkDestroyDescriptorSetLayout(device.GetLogical(), m_DescriptorSetLayout, VK_NULL_HANDLE);
  }

}