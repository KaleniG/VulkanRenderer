#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/EngineComponents/FrameBuffer.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  FrameBufferStructure::FrameBufferStructure(const RenderPassData& data)
    : r_RenderPassData(data) {}

  void FrameBufferStructure::AddView(const Ref<Attachment>& attachment)
  {
    CORE_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    CORE_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  void FrameBufferStructure::AddView(const Ref<ColorAttachment>& attachment)
  {
    CORE_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    CORE_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  void FrameBufferStructure::AddView(const Ref<DepthStencilAttachment>& attachment)
  {
    CORE_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    CORE_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  void FrameBufferStructure::AddView(const Ref<ResolveAttachment>& attachment)
  {
    CORE_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    CORE_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  void FrameBufferStructure::AddView(const Ref<InputAttachment>& attachment)
  {
    CORE_ASSERT(FrameBufferStructure::CheckAttachmentCompatibility(attachment->GetAttachmentType()), "[SYSTEM/VULKAN] Invalid attachment in-place");
    CORE_ASSERT(r_RenderPassData.Attachments[m_Data.Views.size()].format == attachment->GetFormat(), "[SYSTEM/VULKAN] Invalid attachment format");
    FrameBufferStructure::CheckAttachmentsExtent(attachment->GetExtent());

    m_Data.Views.push_back(attachment->GetView());
  }

  bool FrameBufferStructure::CheckAttachmentCompatibility(const AttachmentTypeFlags& type)
  {
    uint32_t counter = 0;

    if (type & VKREN_ATTACHMENT_TYPE_INPUT_BIT)
    {
      for (size_t i = 0; i < r_RenderPassData.Subpasses.size(); i++)
      {
        counter += r_RenderPassData.Subpasses[i].colorAttachmentCount;

        if (r_RenderPassData.Subpasses[i].pDepthStencilAttachment)
          counter++;

        counter += r_RenderPassData.ResolveAttachmentsSizes[i];

        for (size_t j = 0; j < r_RenderPassData.Subpasses[i].inputAttachmentCount; j++)
        {
          if (counter == m_Data.Views.size())
            return true;
          else
            counter++;
        }
      }
      CORE_ERROR("[SYSTEM/VULKAN] Invalid input attachment or attachment out of place");
      return false;
    }
    else if (type & VKREN_ATTACHMENT_TYPE_COLOR_BIT)
    {
      for (size_t i = 0; i < r_RenderPassData.Subpasses.size(); i++)
      {
        for (size_t j = 0; j < r_RenderPassData.Subpasses[i].colorAttachmentCount; j++)
        {
          if (counter == m_Data.Views.size())
            return true;
          else
            counter++;
        }

        if (r_RenderPassData.Subpasses[i].pDepthStencilAttachment)
          counter++;

        counter += r_RenderPassData.ResolveAttachmentsSizes[i];

        counter += r_RenderPassData.Subpasses[i].inputAttachmentCount;
      }
      CORE_ERROR("[SYSTEM/VULKAN] Invalid color attachment or attachment out of place");
      return false;
    }
    else if (type & VKREN_ATTACHMENT_TYPE_DEPTH_STENCIL_BIT)
    {
      for (size_t i = 0; i < r_RenderPassData.Subpasses.size(); i++)
      {
        counter += r_RenderPassData.Subpasses[i].colorAttachmentCount;

        if (r_RenderPassData.Subpasses[i].pDepthStencilAttachment)
        {
          if (counter == m_Data.Views.size())
            return true;
        }
        else
          counter++;

        counter += r_RenderPassData.ResolveAttachmentsSizes[i];

        counter += r_RenderPassData.Subpasses[i].inputAttachmentCount;
      }
      CORE_ERROR("[SYSTEM/VULKAN] Invalid depth/stencil attachment or attachment out of place");
      return false;
    }
    else if (type & VKREN_ATTACHMENT_TYPE_RESOLVE_BIT)
    {
      for (size_t i = 0; i < r_RenderPassData.Subpasses.size(); i++)
      {
        counter += r_RenderPassData.Subpasses[i].colorAttachmentCount;

        if (r_RenderPassData.Subpasses[i].pDepthStencilAttachment)
          counter++;

        for (size_t j = 0; j < r_RenderPassData.ResolveAttachmentsSizes[i]; j++)
        {
          if (counter == m_Data.Views.size())
            return true;
          else
            counter++;
        }

        counter += r_RenderPassData.Subpasses[i].inputAttachmentCount;
      }
      CORE_ERROR("[SYSTEM/VULKAN] Invalid depth/stencil attachment or attachment out of place");
      return false;
    }
    CORE_ERROR("[SYSTEM/VULKAN] Invalid attachment type");
    return false;
  }

  void FrameBufferStructure::CheckAttachmentsExtent(const VkExtent3D& extent)
  {
    if (m_Data.Height.has_value() && m_Data.Width.has_value())
    {
      CORE_ASSERT(m_Data.Width.value() == extent.width && m_Data.Height.value() == extent.height, "[SYSTEM/VULKAN] Invalid attachment extent");
    }
    else
    {
      m_Data.Width.emplace(extent.width);
      m_Data.Height.emplace(extent.height);
    }
  }

  FrameBuffer::~FrameBuffer()
  {
    vkDestroyFramebuffer(Renderer::GetDevice().GetLogical(), m_Framebuffer, VK_NULL_HANDLE);
  }

  Ref<FrameBuffer> FrameBuffer::Create(const Ref<RenderPass>& rendepass, FrameBufferStructure& structure)
  {
    Ref<FrameBuffer> framebuffer = CreateRef<FrameBuffer>();

    framebuffer->r_RenderPass = rendepass;

    FrameBufferData data = structure.GetData();

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = framebuffer->r_RenderPass->Get();
    createInfo.attachmentCount = static_cast<uint32_t>(data.Views.size());
    createInfo.pAttachments = data.Views.data();
    createInfo.width = data.Width.value();
    createInfo.height = data.Height.value();
    createInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(Renderer::GetDevice().GetLogical(), &createInfo, VK_NULL_HANDLE, &framebuffer->m_Framebuffer);
    CORE_ASSERT(result == VK_SUCCESS, "[VULKAN] Failed to create a swapchain framebuffer. Error: {}", Utils::VkResultToString(result));

    return framebuffer;
  }

}