#pragma once

#include <vulkan/vulkan.h>

namespace vkren
{

  class PipelineCache
  {
  public:
    ~PipelineCache();

    const VkPipelineCache& Get() const { return m_Cache; }

    static Ref<PipelineCache> Create(const std::filesystem::path& filepath);

  private:
    VkPipelineCache m_Cache = VK_NULL_HANDLE;
    std::vector<char> m_CacheData;
    std::filesystem::path m_Filepath;
  };

}