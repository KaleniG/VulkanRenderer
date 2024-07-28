#include <vkrenpch.h>

#include <tiny_obj_loader.h>

#include "VulkanRenderer/Renderer/Vertex.h"
#include "VulkanRenderer/Renderer/Model.h"

namespace vkren
{

  Model::Model(const std::filesystem::path& filepath)
  {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.string().c_str());
    if (!warn.empty()) CORE_WARN("[TINY-OBJ-LOADER] {}", warn.substr(0, warn.size() - 1));
    CORE_ASSERT(result, "[TINY-OBJ-LOADER] Failed to load the .obj model, {}", err);

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const tinyobj::shape_t& shape : shapes) 
    {
      for (const tinyobj::index_t& index : shape.mesh.indices) 
      {
        Vertex vertex{};

        vertex.Position = 
        {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]
        };

        vertex.TextureCoord = 
        {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
        };

        vertex.Color = 
        { 
          1.0f, 1.0f, 1.0f 
        };

        if (uniqueVertices.count(vertex) == 0) 
        {
          uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
          m_Vertices.push_back(vertex);
        }

        m_Indices.push_back(uniqueVertices[vertex]);
      }
    }
  }

}