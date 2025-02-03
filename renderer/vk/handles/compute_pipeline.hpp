#pragma once

#include "pipeline.hpp"

#include <crtp.hpp>

namespace renderer::vk {

BEGIN_DECLARE_VKSTRUCT(ComputePipelineCreateInfo, VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineCreateFlags, flags)
    VKSTRUCT_PROPERTY(PipelineShaderStageCreateInfo, stage)
    VKSTRUCT_PROPERTY(VkPipelineLayout, layout)
    VKSTRUCT_PROPERTY(VkPipeline, basePipelineHandle)
    VKSTRUCT_PROPERTY(int32_t, basePipelineIndex)
END_DECLARE_VKSTRUCT();

template <typename T>
struct ComputePipelineFunctions : public CRTPBase<T>
{};

template <typename T>
struct ComputePipelineGroupFunctions : public CRTPBase<T>
{};

using VkComputePipeline = VkPipeline;

namespace handles {
DECLARE_HANDLE_TYPE_FULL_IMPL(ComputePipeline,
    vkCreateComputePipelines,
    vkDestroyPipeline,
    ComputePipelineFunctions,
    ComputePipelineGroupFunctions,
    true);
}

}    //  namespace renderer::vk
