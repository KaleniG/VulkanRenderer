#pragma once

namespace vkren
{

  struct Vertex;
  class Model
  {
  public:
    Model() = default;
    Model(const std::filesystem::path& filepath);

    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

    static Ref<Model> Create(const std::filesystem::path& filepath);

  private:
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
  };

}