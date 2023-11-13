#include "compute_pipeline.hpp"

#include "device.hpp"

namespace vk { namespace handles {

HandleVector<ComputePipeline> ComputePipeline::create(const Device& device,
    VkPipelineCache cache,
    std::span<const ComputePipelineCreateInfo> createInfos)
{
    HandleVector<ComputePipeline> result(createInfos.size(), device, cache);
    vkCreateComputePipelines(device, cache, static_cast<uint32_t>(createInfos.size()),
        createInfos.data(), nullptr, result.handleData());

	return result;
}

ComputePipeline::ComputePipeline(ComputePipeline&& other)
    : Pipeline(std::move(other))
{}

ComputePipeline::ComputePipeline(
    const Device& device, VkPipelineCache cache, VkHandleType* handlePtr)
    : Pipeline(device, cache, handlePtr)
{}

ComputePipeline::ComputePipeline(const Device& device,
    VkPipelineCache cache,
    ComputePipelineCreateInfo createInfo,
    VkHandleType* handlePtr)
    : ComputePipeline(device, cache, handlePtr)
{
    ASSERT(Handle::create(vkCreateComputePipelines, device, cache, 1, &createInfo, nullptr) ==
        VK_SUCCESS);
}

}}    //  namespace vk::handles
