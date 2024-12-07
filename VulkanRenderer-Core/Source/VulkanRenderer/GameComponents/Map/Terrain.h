#pragma once

#include <glm/glm.hpp>

#include "VulkanRenderer/GameComponents/Map/Heightmap.h"
#include "VulkanRenderer/Renderer/Resources/VertexBuffer.h"
#include "VulkanRenderer/Renderer/Resources/IndexBuffer.h"
#include "VulkanRenderer/Renderer/Resources/Texture.h"

namespace vkren
{

  class Terrain
  {
  public:
    Terrain() = default;
    Terrain(glm::uvec2 size, uint32_t start_height, const Ref<Texture>& default_texture);
    Terrain(const Heightmap& heightmap, const Ref<Texture>& default_texture);

    void RaiseTerrain(glm::uvec2 position, float raise_range, float feather_range, float amount);

    const Ref<VertexBuffer>& GetVertexBuffer();
    const Ref<IndexBuffer>& GetIndexBuffer();

    void ExpandHeightmap(DirectionMask direction, glm::uvec2::value_type amount);
    void ResizeHeightmap(glm::uvec2 size);

  private:
    void CalculateVerticesAndIndices();

  private:
    Heightmap m_Heightmap;
    Ref<Texture> r_DefaultTexture;
    Ref<VertexBuffer> m_VertexBuffer;
    Ref<IndexBuffer> m_IndexBuffer;

    bool m_Saved = false;
  };

}