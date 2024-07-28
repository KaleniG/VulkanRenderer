#include <vkrenpch.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VulkanRenderer/Renderer/UniformBuffer.h"
#include "VulkanRenderer/Renderer/Swapchain.h"
#include "VulkanRenderer/Renderer/Renderer.h"

namespace vkren
{

  UniformBuffer::UniformBuffer(size_t buffer_size)
    : r_Device(Renderer::GetDeviceRef()), m_Size(buffer_size)
  {
    Device& device = *r_Device.get();

    device.CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_Memory);
    vkMapMemory(device.GetLogical(), m_Memory, 0, buffer_size, 0, &m_MappedBuffer);
  }

  UniformBuffer::~UniformBuffer()
  {
    Device& device = *r_Device.get();

    vkDestroyBuffer(device.GetLogical(), m_Buffer, VK_NULL_HANDLE);
    vkFreeMemory(device.GetLogical(), m_Memory, VK_NULL_HANDLE);
  }

  void UniformBuffer::Update(VkExtent2D target_extent)
  {
    static auto start_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

    UniformBufferObject ubo;
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), target_extent.width / static_cast<float>(target_extent.height), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    std::memcpy(m_MappedBuffer, &ubo, m_Size);
  }

}