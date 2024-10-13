#pragma once

namespace vkren
{

  struct Vertex;
  class Model
  {
  public:
    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

    static Ref<Model> Create(const std::filesystem::path& obj, const std::filesystem::path& mtl);

  private:
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
  };

}