#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/GraphicsPipeline.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Vertex.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  GraphicsPipeline::GraphicsPipeline(Ref<Shader> shader)
    : r_Device(Renderer::GetDeviceRef()), r_Shader(shader)
  {
    GraphicsPipeline::CreatePipeline();
    GraphicsPipeline::CreateDescriptorPool();
    GraphicsPipeline::CreateDescriptorSets();
  }

  GraphicsPipeline::~GraphicsPipeline()
  {
    GraphicsPipeline::Destroy();
  }

  const VkPipelineLayout& GraphicsPipeline::GetLayout() const
  {
    CORE_ASSERT(m_Layout != VK_NULL_HANDLE, "[VULKAN/SYSTEM] Graphics pipeline layout is invalid");
    return m_Layout;
  }

  const VkPipeline& GraphicsPipeline::Get() const
  {
    CORE_ASSERT(m_Pipeline != VK_NULL_HANDLE, "[VULKAN/SYSTEM] The graphics pipeline is invalid");
    return m_Pipeline;
  }

  void GraphicsPipeline::Destroy()
  {
    Device& device = *r_Device.get();

    CORE_ASSERT(m_Pipeline != VK_NULL_HANDLE, "[VULKAN/SYSTEM] The graphics pipeline is invalid");
    vkDestroyPipeline(device.GetLogical(), m_Pipeline, VK_NULL_HANDLE);

    CORE_ASSERT(m_Layout != VK_NULL_HANDLE, "[VULKAN/SYSTEM] Graphics pipeline layout is invalid");
    vkDestroyPipelineLayout(device.GetLogical(), m_Layout, VK_NULL_HANDLE);

    CORE_ASSERT(m_DescriptorPool != VK_NULL_HANDLE, "[VULKAN/SYSTEM] Descriptor Pool is invalid");
    vkDestroyDescriptorPool(device.GetLogical(), m_DescriptorPool, VK_NULL_HANDLE);

    m_Pipeline = VK_NULL_HANDLE;
    m_Layout = VK_NULL_HANDLE;
    m_DescriptorPool = VK_NULL_HANDLE;
  }

  void GraphicsPipeline::CreatePipeline()
  {
    Device& device = *r_Device.get();

    VkShaderModule vertShaderModule = r_Shader->GetVertShaderModule();
    VkShaderModule fragShaderModule = r_Shader->GetFragShaderModule();

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // VIEWPORT AND SCISSORS
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // RASTERIZER
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    // MULTISAMPLING
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // COLOR BLENDING
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // DEPTH-STENCIL
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    // DYNAMIC STATES
    std::vector<VkDynamicState> dynamicStates =
    {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // PIPELINE LAYOUT
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &r_Shader->GetDescriptorSetLayout();

    VkResult result = vkCreatePipelineLayout(device.GetLogical(), &pipelineLayoutInfo, VK_NULL_HANDLE, &m_Layout);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the pipeline layout. Error: {}", Utils::VkResultToString(result));

    // PIPELINE
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_Layout;
    pipelineInfo.renderPass = device.GetRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(device.GetLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, VK_NULL_HANDLE, &m_Pipeline);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the pipeline. Error: {}", Utils::VkResultToString(result));

    vkDestroyShaderModule(device.GetLogical(), vertShaderModule, VK_NULL_HANDLE);
    vkDestroyShaderModule(device.GetLogical(), fragShaderModule, VK_NULL_HANDLE);
  }

  void GraphicsPipeline::CreateDescriptorPool()
  {
    Device& device = *r_Device.get();

    /* SHOULD WORK ON THAT & MAYBE MOVE THIS TO THE DEVICE CLASS
    std::vector<DescriptorInfo> descriptorInfos = r_Shader->GetDescriptorInfos();
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes(descriptorInfos.size());

    for (int i = 0; i < descriptorPoolSizes.size(); i++)
    {
      descriptorPoolSizes[i].type = descriptorInfos[i].Type;
      descriptorPoolSizes[i].descriptorCount = static_cast<uint32_t>(device.GetConfig().MaxFramesInFlight);
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(device.GetConfig().MaxFramesInFlight);

    VkResult result = vkCreateDescriptorPool(device.GetLogical(), &descriptorPoolCreateInfo, VK_NULL_HANDLE, &m_DescriptorPool);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the descriptor pool. Error: {}", Utils::VkResultToString(result));
    */

    std::vector<VkDescriptorPoolSize> poolSizes =
    {
      { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
      { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
      { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
      { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.maxSets = 1000 * poolSizes.size();
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
    VkResult result = vkCreateDescriptorPool(device.GetLogical(), &descriptorPoolCreateInfo, VK_NULL_HANDLE, &m_DescriptorPool);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create the descriptor pool. Error: {}", Utils::VkResultToString(result));
  }

#ifndef STATUS_DEBUG // somehow optimization leads to a problem where data in the vector gets mixed
#pragma optimize("", off) 
#endif
  void GraphicsPipeline::CreateDescriptorSets()
  {
    Device& device = *r_Device.get();

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts(device.GetConfig().MaxFramesInFlight, r_Shader->GetDescriptorSetLayout());

    VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
    descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocInfo.descriptorPool = m_DescriptorPool;
    descriptorSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    descriptorSetAllocInfo.pSetLayouts = descriptorSetLayouts.data();

    m_DescriptorSets.resize(device.GetConfig().MaxFramesInFlight);
    VkResult result = vkAllocateDescriptorSets(device.GetLogical(), &descriptorSetAllocInfo, m_DescriptorSets.data());
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to allocate the descriptor sets. Error: {}", Utils::VkResultToString(result));

    std::vector<DescriptorInfo> descriptorInfos = r_Shader->GetDescriptorInfos();

    for (size_t i = 0; i < m_DescriptorSets.size(); i++)
    {
      std::vector<VkWriteDescriptorSet> descriptorSetWrites(descriptorInfos.size());

      for (size_t j = 0; j < descriptorSetWrites.size(); j++)
      {
        switch (descriptorInfos[j].Type)
        {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        {
          VkDescriptorBufferInfo descriptorBufferInfo{};
          descriptorBufferInfo.buffer = descriptorInfos[j].UniformBuffers[i]->Get();
          descriptorBufferInfo.offset = 0;
          descriptorBufferInfo.range = descriptorInfos[j].UniformBuffers[i]->GetSize();

          descriptorSetWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
          descriptorSetWrites[j].dstSet = m_DescriptorSets[i];
          descriptorSetWrites[j].dstBinding = descriptorInfos[j].Binding;
          descriptorSetWrites[j].dstArrayElement = 0;
          descriptorSetWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
          descriptorSetWrites[j].descriptorCount = 1;
          descriptorSetWrites[j].pBufferInfo = &descriptorBufferInfo;
          break;
        }
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        {
          VkDescriptorImageInfo descriptorImageInfo{};
          descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
          descriptorImageInfo.imageView = descriptorInfos[j].Texture->GetImageView();
          descriptorImageInfo.sampler = descriptorInfos[j].Texture->GetSampler();

          descriptorSetWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
          descriptorSetWrites[j].dstSet = m_DescriptorSets[i];
          descriptorSetWrites[j].dstBinding = descriptorInfos[j].Binding;
          descriptorSetWrites[j].dstArrayElement = 0;
          descriptorSetWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
          descriptorSetWrites[j].descriptorCount = 1;
          descriptorSetWrites[j].pImageInfo = &descriptorImageInfo;
          break;
        }
        default:
        {
          CORE_ASSERT(false, "[SYSTEM] Invalid descriptor type specified");
          break;
        }
        }
      }

      vkUpdateDescriptorSets(device.GetLogical(), static_cast<uint32_t>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0, VK_NULL_HANDLE);
    }
  }
#ifndef STATUS_DEBUG
#pragma optimize("", on)
#endif // somehow optimization leads to a problem where data in the vector gets mixed

}
