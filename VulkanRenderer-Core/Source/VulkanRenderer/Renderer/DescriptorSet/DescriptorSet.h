#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Renderer/DescriptorSet/DescriptorSetLayout.h"
#include "VulkanRenderer/Renderer/DescriptorSet/DescriptorPool.h"
#include "VulkanRenderer/Renderer/Resources/MStorageTexelBuffer.h"
#include "VulkanRenderer/Renderer/Resources/StorageTexelBuffer.h"
#include "VulkanRenderer/Renderer/Resources/UniformTexelBuffer.h"
#include "VulkanRenderer/Renderer/Resources/MStorageBuffer.h"
#include "VulkanRenderer/Renderer/Resources/MUniformBuffer.h"
#include "VulkanRenderer/Renderer/Resources/StorageBuffer.h"
#include "VulkanRenderer/Renderer/Resources/UniformBuffer.h"
#include "VulkanRenderer/Renderer/Resources/SampledImage.h"
#include "VulkanRenderer/Renderer/Resources/StorageImage.h"

namespace vkren
{

  class DescriptorSetUpdateData
  {
  public:
    void Write(uint32_t binding, const MStorageTexelBuffer& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const StorageTexelBuffer& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const UniformTexelBuffer& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const MStorageBuffer& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const StorageBuffer& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const MUniformBuffer& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const UniformBuffer& buffer, uint32_t array_index = 0);
    void Write(uint32_t binding, const StorageImage& image, uint32_t array_index = 0);
    void Write(uint32_t binding, const SampledImage& image, uint32_t array_index = 0);

    // TODO:  DYNAMIC STORAGE NAD UNIFORM BUFFERS
    //        INPUT ATTACHMENT
    //        INLINE UNIFORM BLOCK

    void Write(uint32_t binding, const MStorageTexelBuffer* buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const StorageTexelBuffer* buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const UniformTexelBuffer* buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const MStorageBuffer* buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const StorageBuffer* buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const MUniformBuffer* buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const UniformBuffer* buffers, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const StorageImage* images, uint32_t count, uint32_t array_index = 0);
    void Write(uint32_t binding, const SampledImage* images, uint32_t count, uint32_t array_index = 0);

    void Reset();

    const std::vector<VkWriteDescriptorSet>& GetWrites() const { return m_WriteData; }

  private:
    std::vector<VkWriteDescriptorSet> m_WriteData;
  };

  struct DescriptorSetCopyInfo
  {
    uint32_t SrcArrayIndex = 0;
    uint32_t DstArrayIndex = 0;
    uint32_t ElementCount = 1;
  };

  class DescriptorSet
  {
  public:
    ~DescriptorSet();
    DescriptorSet(const Ref<DescriptorSetLayout>& layout, const Ref<DescriptorPool>& pool);

    const VkDescriptorSet& Get() const { return m_DescriptorSet; }

    void Update(const DescriptorSetUpdateData& data);

    static void Copy(const DescriptorSet& src, uint32_t src_binding, const DescriptorSet& dst, uint32_t dst_binding, const DescriptorSetCopyInfo& info = {});

    const DescriptorSetLayoutBinding& operator[](uint32_t binding) const;

  private:
    bool CheckWriteData(const VkWriteDescriptorSet& data);
    static bool CheckCopyData(const DescriptorSet& src, uint32_t src_binding, const DescriptorSet& dst, uint32_t dst_binding, const DescriptorSetCopyInfo& info);

  private:
    VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
    Ref<DescriptorPool> m_Pool;
    Ref<DescriptorSetLayout> m_Layout;
  };

}