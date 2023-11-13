#include "graphics_pipeline.hpp"

#include "device.hpp"

namespace vk { namespace handles {

HandleVector<GraphicsPipeline> GraphicsPipeline::create(const Device& device,
    VkPipelineCache cache,
	std::span<const GraphicsPipelineCreateInfo> createInfos)
{
	HandleVector<GraphicsPipeline> result(createInfos.size(), device, cache);
    vkCreateGraphicsPipelines(device, cache, static_cast<uint32_t>(createInfos.size()),
        createInfos.data(), nullptr, result.handleData());

	return result;
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other)
    : Pipeline(std::move(other))
{}

GraphicsPipeline::GraphicsPipeline(
    const Device& device, VkPipelineCache cache, VkHandleType* handlePtr)
    : Pipeline(device, cache, handlePtr)
{}

GraphicsPipeline::GraphicsPipeline(const Device& device,
    VkPipelineCache cache,
    GraphicsPipelineCreateInfo createInfo,
    VkHandleType* handlePtr)
	: GraphicsPipeline(device, cache, handlePtr)
{
    ASSERT(Handle::create(vkCreateGraphicsPipelines, device, cache, 1, &createInfo, nullptr) ==
        VK_SUCCESS);
}

}}    //  namespace vk::handles
