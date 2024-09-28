#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/EngineComponents/RenderPass.h"
#include "VulkanRenderer/Renderer/Resources/Attachment.h"

namespace vkren
{

  struct FrameBufferData
  {
    std::vector<VkImageView> Views;
    uint32_t Width;
    uint32_t Height;
  };

  class FrameBufferStructure
  {
  public:
    FrameBufferStructure(const RenderPassData& data);

    void AddView(const Attachment& attachment);

    const FrameBufferData& GetData() const { return m_Data; }

  private:
    bool CheckAttachmentCompatibility(const AttachmentTypeFlags& type);
    bool CheckAttachmentsExtent();

  private:
    RenderPassData r_RenderPassData;
    std::vector<VkExtent3D> m_AttachmentExtents;
    FrameBufferData m_Data;
  };

  class FrameBuffer
  {
  public:
    ~FrameBuffer();

    Ref<FrameBuffer> Create(const Ref<RenderPass>& rendepass, const FrameBufferStructure& structure);

  private:
    Ref<RenderPass> r_RenderPass;

    VkFramebuffer m_Framebuffer;
  };

}