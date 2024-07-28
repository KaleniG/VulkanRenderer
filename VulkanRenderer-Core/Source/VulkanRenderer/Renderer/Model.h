#pragma once

#include "VulkanRenderer/Renderer/Vertex.h"

namespace vkren
{

  class Model
  {
  public:
    Model(const std::filesystem::path& filepath);

    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

  private:
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
  };

}