#pragma once

#include <boost/pfr.hpp>
#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/Pipeline/ShaderM.h"

namespace vkren
{

  class PipelineShaders
  {
  public:
    ~PipelineShaders();

    void AddShader(const ShaderM& shader, const std::string& entry_point);
    template<typename T> void AddSpecializationConstantBlock(const T& data);

    const std::vector<VkPipelineShaderStageCreateInfo>& GetData() const { return m_Data; }

  private:
    bool CheckForShaderCount(ShaderType type);
    VkShaderStageFlagBits StageFromShaderType(ShaderType type);

  private:
    std::vector<VkPipelineShaderStageCreateInfo> m_Data;
    std::vector<VkSpecializationInfo> m_SpecializationInfos;
    std::vector<std::vector<VkSpecializationMapEntry>> m_MapEntries;
  };

  class VertexInput
  {
  public:
    void AddBinding(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate);
    void AddAttribute(uint32_t location, VkFormat format, uint32_t offset);

    const std::vector<VkVertexInputBindingDescription>& GetBindings() const { return m_VertexInputBindings; }
    const std::vector<VkVertexInputAttributeDescription>& GetAttributes() const { return m_VertexInputAttributes; }

  private:
    std::vector<VkVertexInputBindingDescription> m_VertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributes;
  };

}

#include "VulkanRenderer/Renderer/Pipeline/Pipeline.inl"