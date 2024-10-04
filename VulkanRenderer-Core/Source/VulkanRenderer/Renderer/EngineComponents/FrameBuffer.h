#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"
#include "VulkanRenderer/Renderer/EngineComponents/RenderPass.h"
#include "VulkanRenderer/Renderer/Resources/DepthStencilAttachment.h"
#include "VulkanRenderer/Renderer/Resources/ResolveAttachment.h"
#include "VulkanRenderer/Renderer/Resources/ColorAttachment.h"
#include "VulkanRenderer/Renderer/Resources/InputAttachment.h"
#include "VulkanRenderer/Renderer/Resources/Attachment.h"

namespace vkren
{

  struct FrameBufferData
  {
    std::vector<VkImageView> Views;
    std::optional<uint32_t> Width;
    std::optional<uint32_t> Height;
  };

  class FrameBufferStructure
  {
  public:
    FrameBufferStructure(const RenderPassData& data);

    void AddView(const Ref<Attachment>& attachment);
    void AddView(const Ref<ColorAttachment>& attachment);
    void AddView(const Ref<DepthStencilAttachment>& attachment);
    void AddView(const Ref<ResolveAttachment>& attachment);
    void AddView(const Ref<InputAttachment>& attachment);

    const FrameBufferData& GetData() const { return m_Data; }

  private:
    bool CheckAttachmentCompatibility(const AttachmentTypeFlags& type);
    void CheckAttachmentsExtent(const VkExtent3D& extent);

  private:
    const RenderPassData& r_RenderPassData;
    FrameBufferData m_Data;
  };

  class FrameBuffer
  {
  public:
    ~FrameBuffer();

    static Ref<FrameBuffer> Create(const Ref<RenderPass>& rendepass, FrameBufferStructure& structure);

  private:
    Ref<RenderPass> r_RenderPass;

    VkFramebuffer m_Framebuffer;
  };

}