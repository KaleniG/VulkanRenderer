#pragma once

#include <vulkan/vulkan.h>

namespace vkren
{

  struct QueueInfo
  {
    uint32_t FamilyIndex;
    VkQueue Queue;
  };

}