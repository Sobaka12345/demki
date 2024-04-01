#include "compute_pipeline.hpp"

#include "device.hpp"
#include <map>

namespace renderer::vk { namespace handles {

HandleVector<ComputePipeline> ComputePipeline::create(const Device& device,
    VkPipelineCache cache,
    std::span<const ComputePipelineCreateInfo> createInfos)
{
    HandleVector<ComputePipeline> result;

    result.emplaceBackBatch(vkCreateComputePipelines, createInfos.size(),
        std::forward_as_tuple(device.handle(), cache, static_cast<uint32_t>(createInfos.size()),
            createInfos.data(), nullptr),
        std::forward_as_tuple(device, cache, createInfos));

    return result;
}

ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept
    : Pipeline(std::move(other))
{}

ComputePipeline::ComputePipeline(const Device& device,
    VkPipelineCache cache,
    ComputePipelineCreateInfo createInfo,
    VkHandleType* handlePtr) noexcept
    : Pipeline(device, cache, handlePtr)
{
    ASSERT(Handle::create(vkCreateComputePipelines, device, cache, 1, &createInfo, nullptr) ==
        VK_SUCCESS);
}

ComputePipeline::ComputePipeline(
    const Device& device, VkPipelineCache cache, ComputePipelineCreateInfo createInfo) noexcept
    : ComputePipeline(device, cache, std::move(createInfo), nullptr)
{}

}}    //  namespace renderer::vk::handles
