#pragma once

#include "utils.hpp"

namespace vk { namespace handles {


BEGIN_DECLARE_VKSTRUCT(GraphicsPipelineCreateInfo, VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, stageCount)
    VKSTRUCT_PROPERTY(const VkPipelineShaderStageCreateInfo*, pStages)
    VKSTRUCT_PROPERTY(const VkPipelineVertexInputStateCreateInfo*, pVertexInputState)
    VKSTRUCT_PROPERTY(const VkPipelineInputAssemblyStateCreateInfo*, pInputAssemblyState)
    VKSTRUCT_PROPERTY(const VkPipelineTessellationStateCreateInfo*, pTessellationState)
    VKSTRUCT_PROPERTY(const VkPipelineViewportStateCreateInfo*, pViewportState)
    VKSTRUCT_PROPERTY(const VkPipelineRasterizationStateCreateInfo*, pRasterizationState)
    VKSTRUCT_PROPERTY(const VkPipelineMultisampleStateCreateInfo*, pMultisampleState)
    VKSTRUCT_PROPERTY(const VkPipelineDepthStencilStateCreateInfo*, pDepthStencilState)
    VKSTRUCT_PROPERTY(const VkPipelineColorBlendStateCreateInfo*, pColorBlendState)
    VKSTRUCT_PROPERTY(const VkPipelineDynamicStateCreateInfo*, pDynamicState)
    VKSTRUCT_PROPERTY(VkPipelineLayout, layout)
    VKSTRUCT_PROPERTY(VkRenderPass, renderPass)
    VKSTRUCT_PROPERTY(uint32_t, subpass)
    VKSTRUCT_PROPERTY(VkPipeline, basePipelineHandle)
    VKSTRUCT_PROPERTY(int32_t, basePipelineIndex)
END_DECLARE_VKSTRUCT()

class Device;

class GraphicsPipeline : public Handle<VkPipeline>
{
public:
    static HandleVector<GraphicsPipeline> create(const Device& device,
        VkPipelineCache cache,
		std::span<const GraphicsPipelineCreateInfo> createInfos);

	GraphicsPipeline(const GraphicsPipeline& other) = delete;
	GraphicsPipeline(GraphicsPipeline&& other);
	GraphicsPipeline(const Device& device, VkPipelineCache cache, VkHandleType* handlePtr);
    GraphicsPipeline(const Device& device,
        VkPipelineCache cache,
        GraphicsPipelineCreateInfo createInfo,
        VkHandleType* handlePtr = nullptr);
	~GraphicsPipeline();

private:
	const Device& m_device;
	VkPipelineCache m_cache;
};

}}    //  namespace vk::handles
