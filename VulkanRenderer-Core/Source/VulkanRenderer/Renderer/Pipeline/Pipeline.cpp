#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Pipeline/Pipeline.h"

namespace vkren
{

  PipelineShaders::~PipelineShaders()
  {
    for (auto& specInfo : m_SpecializationInfos)
      if (specInfo.pData)
        delete specInfo.pData;
  }

  void PipelineShaders::AddShader(const ShaderM& shader, const std::string& entry_point)
  {
    CORE_ASSERT(PipelineShaders::CheckForShaderCount(shader.GetType()), "[SYSTEM/VULKAN] Can't have multiple shaders of the same type in a single pipeline");

    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.module = shader.GetModule();
    createInfo.pName = entry_point.c_str();
    createInfo.stage = PipelineShaders::StageFromShaderType(shader.GetType());
    createInfo.pSpecializationInfo = VK_NULL_HANDLE;

    m_Data.push_back(createInfo);
  }

  bool PipelineShaders::CheckForShaderCount(ShaderType type)
  {
    for (const VkPipelineShaderStageCreateInfo& createInfo : m_Data)
      if (createInfo.stage == PipelineShaders::StageFromShaderType(type))
        return false;
    return true;
  }

  VkShaderStageFlagBits PipelineShaders::StageFromShaderType(ShaderType type)
  {
    switch (type)
    {
      case ShaderType::Vertex:                  return VK_SHADER_STAGE_VERTEX_BIT;
      case ShaderType::TessellationControl:     return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
      case ShaderType::TessellationEvaluation:  return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
      case ShaderType::Geometry:                return VK_SHADER_STAGE_GEOMETRY_BIT;
      case ShaderType::Fragment:                return VK_SHADER_STAGE_FRAGMENT_BIT;
      case ShaderType::Compute:                 return VK_SHADER_STAGE_COMPUTE_BIT;
      case ShaderType::RayGeneration:           return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
      case ShaderType::AnyHit:                  return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
      case ShaderType::ClosestHit:              return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
      case ShaderType::Miss:                    return VK_SHADER_STAGE_MISS_BIT_KHR;
      case ShaderType::Intersection:            return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
      case ShaderType::Callable:                return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
      case ShaderType::Task:                    return VK_SHADER_STAGE_TASK_BIT_EXT;
      case ShaderType::Mesh:                    return VK_SHADER_STAGE_MESH_BIT_EXT;
    }
  }

  void VertexInput::AddBinding(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate)
  {
    m_VertexInputBindings.push_back({ binding, stride, input_rate });
  }

  void VertexInput::AddAttribute(uint32_t location, VkFormat format, uint32_t offset)
  {
    m_VertexInputAttributes.push_back({ location, m_VertexInputBindings.back().binding, format, offset });
  }

}