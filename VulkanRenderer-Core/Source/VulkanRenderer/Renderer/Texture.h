#pragma once

#include <vulkan/vulkan.h>

namespace vkren
{

  class Texture
  {
  public:
    Texture(const std::filesystem::path& filepath);
    ~Texture();

    const VkImageView& GetImageView() const { return m_ImageView; }
    const VkSampler& GetSampler() const { return m_Sampler; }

  private:
    Ref<Device> r_Device;

    std::filesystem::path m_Filepath;
    VkImage m_Image = VK_NULL_HANDLE;
    VkDeviceMemory m_Memory = VK_NULL_HANDLE;
    VkImageView m_ImageView = VK_NULL_HANDLE;
    VkSampler m_Sampler = VK_NULL_HANDLE;
  };

}