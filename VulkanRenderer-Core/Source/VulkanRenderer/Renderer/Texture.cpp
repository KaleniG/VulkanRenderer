#include <vkrenpch.h>

#include <stb/image.h>

#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Texture.h"

namespace vkren
{

  Texture::Texture(const std::filesystem::path& filepath) // PNGs ONLY
    : r_Device(Renderer::GetDeviceRef()), m_Filepath(filepath)
  {
    Device& device = *r_Device.get();

    // IMAGE, MEMORY
    int32_t width, height, channelsCount;
    stbi_uc* pixels = stbi_load(m_Filepath.string().c_str(), &width, &height, &channelsCount, STBI_rgb_alpha);
    CORE_ASSERT(pixels, "[STB] Failed to load the image");

    VkDeviceSize imageSize = width * height * 4; // 4 = color channels count

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    device.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device.GetLogical(), stagingBufferMemory, 0, imageSize, 0, &data);
    std::memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device.GetLogical(), stagingBufferMemory);

    stbi_image_free(pixels);

    device.CreateImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_Memory);
    device.CmdTransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    device.CmdCopyBufferToImage(stagingBuffer, m_Image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    device.CmdTransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device.GetLogical(), stagingBuffer, VK_NULL_HANDLE);
    vkFreeMemory(device.GetLogical(), stagingBufferMemory, VK_NULL_HANDLE);

    // IMAGE VIEW
    m_ImageView = device.CreateImageView(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

    // SAMPLER
    VkPhysicalDeviceProperties physicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(device.GetPhysical(), &physicalDeviceProperties);

    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkResult result = vkCreateSampler(device.GetLogical(), &samplerCreateInfo, VK_NULL_HANDLE, &m_Sampler);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the texture sampler");
  }

  Texture::~Texture()
  {
    Device& device = *r_Device.get();

    vkDestroySampler(device.GetLogical(), m_Sampler, VK_NULL_HANDLE);
    vkDestroyImageView(device.GetLogical(), m_ImageView, VK_NULL_HANDLE);
    vkDestroyImage(device.GetLogical(), m_Image, VK_NULL_HANDLE);
    vkFreeMemory(device.GetLogical(), m_Memory, VK_NULL_HANDLE);
  }

}