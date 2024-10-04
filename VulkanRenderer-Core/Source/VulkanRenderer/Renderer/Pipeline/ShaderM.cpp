#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Pipeline/ShaderM.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  ShaderM::~ShaderM()
  {
    vkDestroyShaderModule(Renderer::GetDevice().GetLogical(), m_Module, VK_NULL_HANDLE);
  }

  ShaderM::ShaderM(const std::filesystem::path& filepath)
    : m_Filepath(filepath), m_Type(ShaderM::ShaderTypeFromFileExtension(filepath))
  {
    std::ifstream shaderFile(filepath.string(), std::ios::ate | std::ios::binary);
    CORE_ASSERT(shaderFile.is_open(), "[STD] Couldn't open the shader file: {}", filepath.string());

    size_t fileSize = static_cast<size_t>(shaderFile.tellg());

    std::vector<char> shaderCode(fileSize);

    shaderFile.seekg(0);
    shaderFile.read(shaderCode.data(), fileSize);
    shaderFile.close();

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(Renderer::GetDevice().GetLogical(), &createInfo, VK_NULL_HANDLE, &m_Module);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the vertex shader module. Error: {}", Utils::VkResultToString(result));
  }

  ShaderType ShaderM::ShaderTypeFromFileExtension(const std::filesystem::path& filepath)
  {
    std::string extension;
    {
      std::string filepathStr = filepath.string();
      extension = filepathStr.substr(filepathStr.find_first_of("."));
      extension = extension.substr(0, extension.find_last_of("."));
    }

    if (!std::strcmp(extension.c_str(), ".vert"))
      return ShaderType::Vertex;
    else if (!std::strcmp(extension.c_str(), ".tesc"))
      return ShaderType::TessellationControl;
    else if (!std::strcmp(extension.c_str(), ".tese"))
      return ShaderType::TessellationEvaluation;
    else if (!std::strcmp(extension.c_str(), ".geom"))
      return ShaderType::Geometry;
    else if (!std::strcmp(extension.c_str(), ".frag"))
      return ShaderType::Fragment;
    else if (!std::strcmp(extension.c_str(), ".comp"))
      return ShaderType::Compute;
    else if (!std::strcmp(extension.c_str(), ".rgen"))
      return ShaderType::RayGeneration;
    else if (!std::strcmp(extension.c_str(), ".rahit"))
      return ShaderType::AnyHit;
    else if (!std::strcmp(extension.c_str(), ".rchit"))
      return ShaderType::ClosestHit;
    else if (!std::strcmp(extension.c_str(), ".rmiss"))
      return ShaderType::Miss;
    else if (!std::strcmp(extension.c_str(), ".rint"))
      return ShaderType::Intersection;
    else if (!std::strcmp(extension.c_str(), ".rcall"))
      return ShaderType::Callable;
    else if (!std::strcmp(extension.c_str(), ".task"))
      return ShaderType::Task;
    else if (!std::strcmp(extension.c_str(), ".mesh"))
      return ShaderType::Mesh;
  }

}