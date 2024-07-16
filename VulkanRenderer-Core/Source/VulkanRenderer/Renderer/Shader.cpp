#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Shader.h"

namespace vkren
{

  void DescriptorSetBindings::AddBinding(VkDescriptorType type, VkShaderStageFlags stage)
  {
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
    descriptorSetLayoutBinding.binding = m_BindingCount;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.descriptorType = type;
    descriptorSetLayoutBinding.stageFlags = stage;
    descriptorSetLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;

    m_Bindings.push_back(descriptorSetLayoutBinding);
    m_UsedBindings.insert(m_BindingCount);

    while (m_UsedBindings.contains(m_BindingCount + 1))
      m_BindingCount++;
    m_BindingCount++;
  }

  void DescriptorSetBindings::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage)
  {
    CORE_ASSERT(!m_UsedBindings.contains(binding), "[SYSTEM] This binding is already taken");

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
    descriptorSetLayoutBinding.binding = binding;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.descriptorType = type;
    descriptorSetLayoutBinding.stageFlags = stage;
    descriptorSetLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;

    m_Bindings.push_back(descriptorSetLayoutBinding);
    m_UsedBindings.insert(m_BindingCount);
    m_BindingCount = binding;
  }

  Shader::Shader(Device& device, const std::filesystem::path& vert_shader, const std::filesystem::path& frag_shader, const DescriptorSetBindings& bindings)
    : r_Device(device), m_VertShaderFilepath(vert_shader), m_FragShaderFilepath(frag_shader)
  {
    std::ifstream vertShaderFile(vert_shader.string(), std::ios::ate | std::ios::binary);
    CORE_ASSERT(vertShaderFile.is_open(), "[STD] Couldn't open the vertex shader");
    size_t vertShaderFileSize = static_cast<size_t>(vertShaderFile.tellg());
    m_VertShaderCode.resize(vertShaderFileSize);
    vertShaderFile.seekg(0);
    vertShaderFile.read(m_VertShaderCode.data(), vertShaderFileSize);
    vertShaderFile.close();

    std::ifstream fragShaderFile(frag_shader.string(), std::ios::ate | std::ios::binary);
    CORE_ASSERT(fragShaderFile.is_open(), "[STD] Couldn't open the fragment shader");
    size_t fragShaderFileSize = static_cast<uint32_t>(fragShaderFile.tellg());
    m_FragShaderCode.resize(fragShaderFileSize);
    fragShaderFile.seekg(0);
    fragShaderFile.read(m_FragShaderCode.data(), fragShaderFileSize);
    fragShaderFile.close();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = bindings.GetCount();
    descriptorSetLayoutCreateInfo.pBindings = bindings.GetData();

    VkResult result = vkCreateDescriptorSetLayout(r_Device.GetLogical(), &descriptorSetLayoutCreateInfo, VK_NULL_HANDLE, &m_DescriptorSetLayout);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the shaders' descriptor set layout");
  }

  VkShaderModule Shader::GetVertShaderModule()
  {
    CORE_ASSERT(!m_VertShaderCode.empty(), "[VULKAN/SYSTEM]  Invalid vertex shader code");

    VkShaderModuleCreateInfo vertShaderModuleCreateInfo{};
    vertShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertShaderModuleCreateInfo.codeSize = m_VertShaderCode.size();
    vertShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(m_VertShaderCode.data());

    VkShaderModule vertShaderModule;
    VkResult result = vkCreateShaderModule(r_Device.GetLogical(), &vertShaderModuleCreateInfo, VK_NULL_HANDLE, &vertShaderModule);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the vertex shader module");

    return vertShaderModule;
  }

  VkShaderModule Shader::GetFragShaderModule()
  {
    CORE_ASSERT(!m_FragShaderCode.empty(), "[VULKAN/SYSTEM] Invalid fragment shader code");

    VkShaderModuleCreateInfo fragShaderModuleCreateInfo{};
    fragShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragShaderModuleCreateInfo.codeSize = m_FragShaderCode.size();
    fragShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(m_FragShaderCode.data());

    VkShaderModule fragShaderModule;
    VkResult result = vkCreateShaderModule(r_Device.GetLogical(), &fragShaderModuleCreateInfo, VK_NULL_HANDLE, &fragShaderModule);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the fragment shader module");

    return fragShaderModule;
  }

  const VkDescriptorSetLayout& Shader::GetDescriptorSetLayout() const
  {
    CORE_ASSERT(m_DescriptorSetLayout != VK_NULL_HANDLE, "[VULKAN/SYSTEM] Invalid descriptor set layout");
    return m_DescriptorSetLayout;
  }

  void Shader::Destroy()
  {
    CORE_ASSERT(m_DescriptorSetLayout != VK_NULL_HANDLE, "[VULKAN/SYSTEM] Invalid descriptor set layout");
    vkDestroyDescriptorSetLayout(r_Device.GetLogical(), m_DescriptorSetLayout, VK_NULL_HANDLE);
  }

}