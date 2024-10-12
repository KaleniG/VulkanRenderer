#pragma once

#include "VulkanRenderer/Renderer/Resources/SampledImage.h"

namespace vkren
{

  class TextureM
  {
  public:
    const Ref<SampledImage>& GetImage() const { return m_Image; }

    static TextureM Create(const std::filesystem::path& filepath, const Ref<Sampler> sampler = nullptr);

  private:
    std::filesystem::path m_Filepath;
    Ref<SampledImage> m_Image;
  };

}