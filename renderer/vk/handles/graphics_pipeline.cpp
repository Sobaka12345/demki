#include "graphics_pipeline.hpp"

#include "device.hpp"

namespace renderer::vk { namespace handles {

HandleVector<GraphicsPipeline> GraphicsPipeline::create(const Device& device,
    VkPipelineCache cache,
	std::span<const GraphicsPipelineCreateInfo> createInfos)
{
    HandleVector<GraphicsPipeline> result;

    result.emplaceBackBatch(vkCreateGraphicsPipelines, createInfos.size(),
        std::forward_as_tuple(device.handle(), cache, static_cast<uint32_t>(createInfos.size()),
            createInfos.data(), nullptr),
        std::forward_as_tuple(device, cache, createInfos));

	return result;
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    : Pipeline(std::move(other))
{}

GraphicsPipeline::GraphicsPipeline(const Device& device,
    VkPipelineCache cache,
    GraphicsPipelineCreateInfo createInfo,
    VkHandleType* handlePtr) noexcept
    : Pipeline(device, cache, handlePtr)
{
    ASSERT(Handle::create(vkCreateGraphicsPipelines, device, cache, 1, &createInfo, nullptr) ==
        VK_SUCCESS);
}

GraphicsPipeline::GraphicsPipeline(
    const Device& device, VkPipelineCache cache, GraphicsPipelineCreateInfo createInfo) noexcept
    : GraphicsPipeline(device, cache, std::move(createInfo), nullptr)
{}


}}    //  namespace renderer::vk::handles
