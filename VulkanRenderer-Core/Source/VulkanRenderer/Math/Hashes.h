#pragma once

#include <glm/glm.hpp>

namespace glm
{
  struct ivec2Hash 
  {
    std::size_t operator()(const glm::ivec2& key) const 
    {
      return std::hash<int>()(key.x) ^ (std::hash<int>()(key.y) << 1);
    }
  };
}