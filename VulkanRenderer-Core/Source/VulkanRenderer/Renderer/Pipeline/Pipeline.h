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

}

#include "VulkanRenderer/Renderer/Pipeline/Pipeline.inl"