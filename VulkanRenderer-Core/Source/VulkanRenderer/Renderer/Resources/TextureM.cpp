#include <vkrenpch.h>

#include <stb/stb_image.h>

#include "VulkanRenderer/Renderer/Resources/StagingBuffer.h"
#include "VulkanRenderer/Renderer/Resources/TextureM.h"

namespace vkren
{

  TextureM TextureM::Create(const std::filesystem::path& filepath, const Ref<Sampler> sampler)
  {
    CORE_ASSERT(!std::strcmp(filepath.extension().string().c_str(), ".png"), "[SYSTEM] Only '.png' image format supported for now");

    TextureM texture;

    texture.m_Filepath = filepath;

    int32_t width, height, channelsCount;
    stbi_uc* pixels = stbi_load(texture.m_Filepath.string().c_str(), &width, &height, &channelsCount, STBI_rgb_alpha);
    CORE_ASSERT(pixels, "[STB] Failed to load the image");

    VkDeviceSize imageSize = width * height * 4;

    StagingBuffer stagingBuffer = StagingBuffer::Create(imageSize);
    stagingBuffer.Map();
    stagingBuffer.Update(pixels);
    stagingBuffer.Unmap();

    stbi_image_free(pixels);

    SampledImageCreateInfo createInfo = {};
    createInfo.Type = VK_IMAGE_TYPE_2D;
    createInfo.Format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.Extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
    if (sampler)
      createInfo.Sampler.Sampler = sampler;
    else
      createInfo.Sampler.SamplerCreateInfo.ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;

    texture.m_Image = SampledImage::Create(createInfo);

    stagingBuffer.CopyToImage(*texture.m_Image.get());

    ImageTransitionSpecifics specifics;
    specifics.PipelineStagesMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    texture.GetImage()->Transition(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, specifics);

    return texture;
  }

}