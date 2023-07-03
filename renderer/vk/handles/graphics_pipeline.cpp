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
	: Handle(std::move(other))
	, m_device(other.m_device)
	, m_cache(other.m_cache)
{}

GraphicsPipeline::GraphicsPipeline(
    const Device& device, VkPipelineCache cache, VkHandleType* handlePtr)
	: Handle(handlePtr)
	, m_device(device)
	, m_cache(cache)
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

GraphicsPipeline::~GraphicsPipeline()
{
	destroy(vkDestroyPipeline, m_device, handle(), nullptr);
}

}}    //  namespace vk::handles
