#pragma once

#include <vulkan/vulkan.h>

namespace vkren
{

  enum class ShaderType
  {
    Vertex,
    TessellationControl,
    TessellationEvaluation,
    Geometry,
    Fragment,
    Compute,
    RayGeneration,
    AnyHit,
    ClosestHit,
    Miss,
    Intersection,
    Callable,
    Task,
    Mesh
  };

  class Shader
  {
  public:
    ~Shader();

    const VkShaderModule& GetModule() const { return m_Module; }
    const ShaderType& GetType() const { return m_Type; }

    static Ref<Shader> Create(const std::filesystem::path& filepath);

  private:
    ShaderType ShaderTypeFromFileExtension(const std::filesystem::path& filepath);

  private:
    VkShaderModule m_Module = VK_NULL_HANDLE;
    ShaderType m_Type;
    std::filesystem::path m_Filepath;
  };

}