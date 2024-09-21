#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/DescriptorSet/DescriptorSet.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  void DescriptorSetUpdateData::Write(uint32_t binding, const MStorageTexelBuffer& buffer, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, buffer, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const MStorageBuffer& buffer, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, buffer, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const UniformBuffer& buffer, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, buffer, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const MUniformBuffer& buffer, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, buffer, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const UniformTexelBuffer& buffer, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, buffer, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const StorageTexelBuffer& buffer, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, buffer, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const StorageBuffer& buffer, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, buffer, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const StorageImage& image, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, image, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const SampledImage& image, uint32_t array_index)
  {
    DescriptorSetUpdateData::Write(binding, image, array_index);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const MStorageTexelBuffer* buffers, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorBufferInfo> bufferInfos(count);
    std::vector<VkBufferView> bufferViews(count);

    for (size_t i = 0; i < count; i++)
    {
      bufferInfos[i].buffer = buffers[i].Get();
      bufferInfos[i].offset = 0;
      bufferInfos[i].range = buffers[i].GetSize();

      bufferViews[i] = buffers[i].GetView();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    writeStructure.pBufferInfo = bufferInfos.data();
    writeStructure.pTexelBufferView = bufferViews.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const MStorageBuffer* buffers, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorBufferInfo> bufferInfos(count);

    for (size_t i = 0; i < count; i++)
    {
      bufferInfos[i].buffer = buffers[i].Get();
      bufferInfos[i].offset = 0;
      bufferInfos[i].range = buffers[i].GetSize();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeStructure.pBufferInfo = bufferInfos.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const UniformBuffer* buffers, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorBufferInfo> bufferInfos(count);

    for (size_t i = 0; i < count; i++)
    {
      bufferInfos[i].buffer = buffers[i].Get();
      bufferInfos[i].offset = 0;
      bufferInfos[i].range = buffers[i].GetSize();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeStructure.pBufferInfo = bufferInfos.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const MUniformBuffer* buffers, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorBufferInfo> bufferInfos(count);

    for (size_t i = 0; i < count; i++)
    {
      bufferInfos[i].buffer = buffers[i].Get();
      bufferInfos[i].offset = 0;
      bufferInfos[i].range = buffers[i].GetSize();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeStructure.pBufferInfo = bufferInfos.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const UniformTexelBuffer* buffers, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorBufferInfo> bufferInfos(count);
    std::vector<VkBufferView> bufferViews(count);

    for (size_t i = 0; i < count; i++)
    {
      bufferInfos[i].buffer = buffers[i].Get();
      bufferInfos[i].offset = 0;
      bufferInfos[i].range = buffers[i].GetSize();

      bufferViews[i] = buffers[i].GetView();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    writeStructure.pBufferInfo = bufferInfos.data();
    writeStructure.pTexelBufferView = bufferViews.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const StorageTexelBuffer* buffers, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorBufferInfo> bufferInfos(count);
    std::vector<VkBufferView> bufferViews(count);

    for (size_t i = 0; i < count; i++)
    {
      bufferInfos[i].buffer = buffers[i].Get();
      bufferInfos[i].offset = 0;
      bufferInfos[i].range = buffers[i].GetSize();

      bufferViews[i] = buffers[i].GetView();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    writeStructure.pBufferInfo = bufferInfos.data();
    writeStructure.pTexelBufferView = bufferViews.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const StorageBuffer* buffers, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorBufferInfo> bufferInfos(count);

    for (size_t i = 0; i < count; i++)
    {
      bufferInfos[i].buffer = buffers[i].Get();
      bufferInfos[i].offset = 0;
      bufferInfos[i].range = buffers[i].GetSize();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeStructure.pBufferInfo = bufferInfos.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const StorageImage* images, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorImageInfo> imageInfos(count);

    for (size_t i = 0; i < count; i++)
    {
      imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
      imageInfos[i].imageView = images[i].GetView();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeStructure.pImageInfo = imageInfos.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Write(uint32_t binding, const SampledImage* images, uint32_t count, uint32_t array_index)
  {
    std::vector<VkDescriptorImageInfo> imageInfos(count);

    for (size_t i = 0; i < count; i++)
    {
      imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfos[i].imageView = images[i].GetView();
      imageInfos[i].sampler = images[i].GetSampler()->Get();
    }

    VkWriteDescriptorSet writeStructure = {};
    writeStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeStructure.dstSet = VK_NULL_HANDLE; // will get defined inside the DescriptorSet class
    writeStructure.dstBinding = binding;
    writeStructure.dstArrayElement = array_index;
    writeStructure.descriptorCount = count;
    writeStructure.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeStructure.pImageInfo = imageInfos.data();

    m_WriteData.push_back(writeStructure);
  }

  void DescriptorSetUpdateData::Reset()
  {
    m_WriteData.clear();
  }

  DescriptorSet::~DescriptorSet()
  {
    VkResult result = vkFreeDescriptorSets(Renderer::GetDevice().GetLogical(), m_Pool->Get(), 1, &m_DescriptorSet);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to deallocate the descriptor sets. Error: {}", Utils::VkResultToString(result)); // LATER CHANGE INTO MULTIPLE DEALLOCATIONS AT ONCE
  }

  DescriptorSet::DescriptorSet(const Ref<DescriptorSetLayout>& layout, const Ref<DescriptorPool>& pool)
    : m_Layout(layout), m_Pool(pool)
  {
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool->Get();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout->Get();

    VkResult result = vkAllocateDescriptorSets(Renderer::GetDevice().GetLogical(), &allocInfo, &m_DescriptorSet);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate the descriptor sets. Error: {}", Utils::VkResultToString(result)); // LATER CHANGE INTO MULTIPLE ALLOCATIONS AT ONCE
  }

  void DescriptorSet::Update(const DescriptorSetUpdateData& data)
  {
    std::vector<VkWriteDescriptorSet> writes = data.GetWrites();

    for (VkWriteDescriptorSet& write : writes)
    {
      CORE_ASSERT(DescriptorSet::CheckWriteData(write), "[SYSTEM] Invalid write data");
      write.dstSet = m_DescriptorSet;
    }

    vkUpdateDescriptorSets(Renderer::GetDevice().GetLogical(), static_cast<uint32_t>(writes.size()), writes.data(), 0, VK_NULL_HANDLE);
  }

  void DescriptorSet::Copy(const DescriptorSet& src, uint32_t src_binding, const DescriptorSet& dst, uint32_t dst_binding, const DescriptorSetCopyInfo& info)
  {
    CORE_ASSERT(DescriptorSet::CheckCopyData(src, src_binding, dst, dst_binding, info), "[SYSTEM/VULKAN] Invalid copy data specified");

    VkCopyDescriptorSet copyStructure = {};
    copyStructure.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
    copyStructure.srcSet = src.Get();
    copyStructure.srcBinding = src_binding;
    copyStructure.srcArrayElement = info.SrcArrayIndex;
    copyStructure.dstSet = dst.Get();
    copyStructure.dstBinding = dst_binding;
    copyStructure.dstArrayElement = info.DstArrayIndex;
    copyStructure.descriptorCount = info.ElementCount;

    vkUpdateDescriptorSets(Renderer::GetDevice().GetLogical(), 0, VK_NULL_HANDLE, 1, &copyStructure);
  }

  const DescriptorSetLayoutBinding& DescriptorSet::operator[](uint32_t binding) const
  {
    for (const DescriptorSetLayoutBinding& layout_binding : m_Layout->GetBindings())
    {
      if (layout_binding.Binding == binding)
        return layout_binding;
    }
    CORE_ASSERT(false, "[SYSTEM] Couldn't find the specified binding '{}'", binding);
  }

  bool DescriptorSet::CheckWriteData(const VkWriteDescriptorSet& data)
  {
    for (const DescriptorSetLayoutBinding& binding : m_Layout->GetBindings())
    {
      if (data.dstBinding == binding.Binding)
      {
        if (data.descriptorType == binding.Type)
        {
          if (data.dstArrayElement + data.descriptorCount <= binding.Count)
          {
            if (binding.ImmutableSamplers)
            {
              if (data.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
              {
                if (data.pImageInfo->sampler)
                {
                  CORE_ERROR("[SYSTEM] No Sampler should be specified when using immutable samplers");
                  return false;
                }
                return true;
              }
              CORE_ERROR("[SYSTEM] Cannot have immutable samplers if the descriptor is not a 'SampledImage'");
              return false;
            }
            return true;
          }
          CORE_ERROR("[SYSTEM] Invalid descriptor offset specified");
          return false;
        }
        CORE_ERROR("[SYSTEM] Invalid descriptor type for the binding index");
        return false;
      }
      continue;
    }
    CORE_ERROR("[SYSTEM] Couldn't find a binding with index: {}", data.dstBinding);
    return false;
  }

  bool DescriptorSet::CheckCopyData(const DescriptorSet& src, uint32_t src_binding, const DescriptorSet& dst, uint32_t dst_binding, const DescriptorSetCopyInfo& info)
  {
    for (const DescriptorSetLayoutBinding& srcBinding : src.m_Layout->GetBindings())
    {
      if (srcBinding.Binding == src_binding)
      {
        for (const DescriptorSetLayoutBinding& dstBinding : dst.m_Layout->GetBindings())
        {
          if (dstBinding.Binding == dst_binding)
          {
            if (srcBinding.Type == dstBinding.Type)
            {
              if (info.SrcArrayIndex + info.ElementCount <= srcBinding.Count || info.DstArrayIndex + info.ElementCount <= dstBinding.Count)
                return true;
              CORE_ERROR("[SYSTEM] Out of array bounds descriptor copy");
              return false;
            }
            CORE_ERROR("[SYSTEM] The source and destination sets' bindings do not match with the descriptor type");
            return false;
          }
          continue;
        }
        CORE_ERROR("[SYSTEM] Couldn't find a binding with index '{}' in the destination descriptor set", dst_binding);
        return false;
      }
      continue;
    }
    CORE_ERROR("[SYSTEM] Couldn't find a binding with index '{}' in the source descriptor set", src_binding);
    return false;
  }

}