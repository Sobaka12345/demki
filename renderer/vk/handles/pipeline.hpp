#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(PipelineShaderStageCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineShaderStageCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkShaderStageFlagBits, stage)
    VKSTRUCT_PROPERTY(VkShaderModule, module)
    VKSTRUCT_PROPERTY(const char*, pName)
    VKSTRUCT_PROPERTY(const VkSpecializationInfo*, pSpecializationInfo)
END_DECLARE_VKSTRUCT();

}    //  namespace renderer::vk
