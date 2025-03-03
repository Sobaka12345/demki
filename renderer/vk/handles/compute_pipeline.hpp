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

using VkComputePipeline = VkPipeline;

template <typename T>
struct ComputePipelineFunctions : public CRTPBase<T>
{
    static constexpr inline void create(VkDevice device,
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const ComputePipelineCreateInfo* pCreateInfos,
        const VkAllocationCallbacks* pAllocator,
        VkPipeline* pPipelines) noexcept
    {
        ASSERT(vkCreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos,
                   pAllocator, pPipelines) == VK_SUCCESS);
    }

    static constexpr inline void destroy(VkDevice device,
        VkPipeline pipeline,
        VkPipelineCache pipelineCache,
        const VkAllocationCallbacks* pAllocator) noexcept
    {
        vkDestroyPipeline(device, pipeline, pAllocator);
    }
};

template <typename T>
struct ComputePipelineGroupFunctions : public CRTPBase<T>
{};

namespace handles {
DECLARE_HANDLE_TYPE(ComputePipeline, ComputePipelineFunctions, ComputePipelineGroupFunctions);
}

}    //  namespace renderer::vk
