#pragma once

#include "pipeline.hpp"

namespace vk {

BEGIN_DECLARE_VKSTRUCT(ComputePipelineCreateInfo, VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineCreateFlags, flags)
    VKSTRUCT_PROPERTY(PipelineShaderStageCreateInfo, stage)
    VKSTRUCT_PROPERTY(VkPipelineLayout, layout)
    VKSTRUCT_PROPERTY(VkPipeline, basePipelineHandle)
    VKSTRUCT_PROPERTY(int32_t, basePipelineIndex)
END_DECLARE_VKSTRUCT();

namespace handles {

class Device;

class ComputePipeline : public Pipeline
{
    HANDLE(ComputePipeline);

public:
    static HandleVector<ComputePipeline> create(const Device& device,
        VkPipelineCache cache,
        std::span<const ComputePipelineCreateInfo> createInfos);

public:
    ComputePipeline(const ComputePipeline& other) = delete;
    ComputePipeline(ComputePipeline&& other) noexcept;
    ComputePipeline(
        const Device& device, VkPipelineCache cache, ComputePipelineCreateInfo createInfo) noexcept;

protected:
    ComputePipeline(const Device& device,
        VkPipelineCache cache,
        ComputePipelineCreateInfo createInfo,
        VkHandleType* handlePtr) noexcept;
};

}    //  namespace handles
}    //  namespace vk
