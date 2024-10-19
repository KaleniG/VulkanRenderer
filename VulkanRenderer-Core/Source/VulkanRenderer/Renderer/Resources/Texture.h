#pragma once

#include "VulkanRenderer/Renderer/Resources/SampledImage.h"

namespace vkren
{

  class Texture
  {
  public:
    const Ref<SampledImage>& GetImage() const { return m_Image; }

    static Texture Create(const std::filesystem::path& filepath, const Ref<Sampler> sampler = nullptr);

  private:
    std::filesystem::path m_Filepath;
    Ref<SampledImage> m_Image;
  };

}