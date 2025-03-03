#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(PipelineLayoutCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineLayoutCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, setLayoutCount)
    VKSTRUCT_PROPERTY(const VkDescriptorSetLayout*, pSetLayouts)
    VKSTRUCT_PROPERTY(uint32_t, pushConstantRangeCount)
    VKSTRUCT_PROPERTY(const VkPushConstantRange*, pPushConstantRanges)
END_DECLARE_VKSTRUCT()

template <typename T>
struct PipelineLayoutFunctions : public CRTPBase<T>
{
    CREATE_FUNC(PipelineLayout, Device)
    DESTROY_FUNC(PipelineLayout, Device)
};

template <typename T>
struct PipelineLayoutGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(PipelineLayout, PipelineLayoutFunctions, PipelineLayoutGroupFunctions);
}

}    //  namespace renderer::vk
