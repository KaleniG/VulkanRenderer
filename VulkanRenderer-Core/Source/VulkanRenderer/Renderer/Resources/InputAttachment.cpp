#include <vkrenpch.h>

#include "VulkanRenderer/Renderer/Resources/InputAttachment.h"
#include "VulkanRenderer/Renderer/Renderer.h"
#include "VulkanRenderer/Renderer/Utils.h"

namespace vkren
{

  InputAttachment::~InputAttachment()
  {
    vkDestroyImageView(Renderer::GetDevice().GetLogical(), m_View, VK_NULL_HANDLE);
  }

}