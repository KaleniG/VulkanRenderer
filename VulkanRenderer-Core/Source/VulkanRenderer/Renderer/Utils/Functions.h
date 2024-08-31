#pragma once

#include <vulkan/vulkan.h>

#include "VulkanRenderer/Core/Base.h"

// New assertion macro
#define ASSERT_PIPELINE_STAGES(stages, valid_stages) CORE_ASSERT((stages) & (valid_stages), "[VULKAN/SYSTEM] Invalid pipeline stages specified");
#define ASSERT_ACCESS_MASKS(access, valid_mask) CORE_ASSERT((access) & (valid_mask), "[VULKAN/SYSTEM] Invalid access masks specified during image layout transition");

namespace vkren
{

  namespace Utils
  {

    VkPipelineStageFlags AccessMaskToPipelineStagesMask(const VkAccessFlags& access, const VkPipelineStageFlags& stages)
    {
      VkPipelineStageFlags dstStages = VK_PIPELINE_STAGE_NONE;

      if (access & VK_ACCESS_INDIRECT_COMMAND_READ_BIT)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
            VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_TRANSFER_BIT |
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_HOST_BIT |
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
      }

      if (access & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT))
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
            VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_TRANSFER_BIT |
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_HOST_BIT |
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
      }

      if (access & VK_ACCESS_UNIFORM_READ_BIT)
      {
        ASSERT_PIPELINE_STAGES(stages, (
          VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
          VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
          VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
          VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
          VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR |
          VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT |
          VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
          VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
          VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
          ));
        dstStages |= stages;
      }

      if (access & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      }

      if (access & (VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT))
      {
        ASSERT_PIPELINE_STAGES(stages, (
          VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
          VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
          VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
          VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
          VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR |
          VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT |
          VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
          VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
          VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
          ));
        dstStages |= stages;
      }

      if (access & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT))
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      }

      if (access & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT))
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      }

      if (access & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT))
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_TRANSFER_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
      }

      if (access & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT))
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_TRANSFER_BIT |
            VK_PIPELINE_STAGE_HOST_BIT |
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_HOST_BIT;
      }

      if (access & (VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT))
      {
        ASSERT_PIPELINE_STAGES(stages, (
          VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
          VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
          VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
          ));
        dstStages |= stages;
      }

      if (access & (VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT | VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT | VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT))
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
            VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_TRANSFER_BIT |
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
      }

      if (access & VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT |
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
            VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
            VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
            VK_PIPELINE_STAGE_TRANSFER_BIT |
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
      }

      if (access & VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      }

      if (access & VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR |
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
      }

      if (access & VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
      }

      if (access & VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
      }

      if (access & VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
      }

      if (access & VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
      }

      if (access & VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV)
      {
        if (stages != VK_PIPELINE_STAGE_NONE)
        {
          ASSERT_PIPELINE_STAGES(stages, (
            VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV |
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
            VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT |
            VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT
            ));
          dstStages |= stages;
        }
        else
          dstStages |= VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
      }

      CORE_ASSERT(dstStages != VK_PIPELINE_STAGE_NONE, "[VULKAN/SYSTEM] Invalid pipeline stages");

      return dstStages;
    }

    VkAccessFlags ImageLayoutToAccessMask(const VkImageLayout& layout, const VkAccessFlags& access)
    {
      VkAccessFlags accessMask = VK_ACCESS_NONE;

      switch (layout)
      {
      case VK_IMAGE_LAYOUT_UNDEFINED:
      {
        break;
      }
      case VK_IMAGE_LAYOUT_GENERAL:
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
          VK_ACCESS_INDEX_READ_BIT |
          VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
          VK_ACCESS_UNIFORM_READ_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_SHADER_READ_BIT |
          VK_ACCESS_SHADER_WRITE_BIT |
          VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_TRANSFER_READ_BIT |
          VK_ACCESS_TRANSFER_WRITE_BIT |
          VK_ACCESS_HOST_READ_BIT |
          VK_ACCESS_HOST_WRITE_BIT |
          VK_ACCESS_MEMORY_READ_BIT |
          VK_ACCESS_MEMORY_WRITE_BIT
        );
        accessMask |= access;
        break;
      }
      case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
            VK_ACCESS_MEMORY_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
            VK_ACCESS_MEMORY_READ_BIT |
            VK_ACCESS_MEMORY_WRITE_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_SHADER_READ_BIT |
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
            VK_ACCESS_MEMORY_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_SHADER_READ_BIT |
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
            VK_ACCESS_MEMORY_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_SHADER_READ_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_TRANSFER_READ_BIT |
            VK_ACCESS_MEMORY_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_TRANSFER_READ_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_TRANSFER_WRITE_BIT |
            VK_ACCESS_MEMORY_WRITE_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
      case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
            VK_ACCESS_SHADER_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
            VK_ACCESS_SHADER_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
            VK_ACCESS_SHADER_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
            VK_ACCESS_SHADER_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
            VK_ACCESS_SHADER_READ_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        break;
      }
      case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_SHADER_READ_BIT |
          VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
          VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_TRANSFER_READ_BIT
        );
        accessMask |= access;
        break;
      }
      case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_MEMORY_READ_BIT |
          VK_ACCESS_MEMORY_WRITE_BIT
        );
        accessMask |= access;
        break;
      }
      case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
      {
        ASSERT_ACCESS_MASKS(access,
          VK_ACCESS_MEMORY_READ_BIT |
          VK_ACCESS_MEMORY_WRITE_BIT |
          VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_TRANSFER_READ_BIT |
          VK_ACCESS_TRANSFER_WRITE_BIT
        );
        accessMask |= access;
        break;
      }
      case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT |
            VK_ACCESS_SHADER_READ_BIT |
            VK_ACCESS_TRANSFER_READ_BIT |
            VK_ACCESS_TRANSFER_WRITE_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
        break;
      }
      case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
      {
        if (access != VK_ACCESS_NONE)
        {
          ASSERT_ACCESS_MASKS(access,
            VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR |
            VK_ACCESS_SHADER_READ_BIT |
            VK_ACCESS_TRANSFER_READ_BIT |
            VK_ACCESS_TRANSFER_WRITE_BIT
          );
          accessMask |= access;
        }
        else
          accessMask |= VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
        break;
      }
      default:
      {
        CORE_ASSERT(false, "[VULKAN/SYSTEM] Unsupported image layout");
        break;
      }
      }

      return accessMask;
    }

  }

}
