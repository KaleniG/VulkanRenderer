#pragma once

#include "VulkanRenderer/Renderer/Resources/AbstractionLayers/Image.h"

namespace vkren
{

  class InputAttachment : public Image
  {
  public:
    ~InputAttachment();

    const VkImageView& GetView() const { return m_View; }
    const VkFramebuffer& GetFramebuffer() const { return m_Framebuffer; }

  private:
    VkImageView m_View = VK_NULL_HANDLE;
    VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
  };

}