#pragma once

#include "handle.hpp"

#include "vk/utils.hpp"

#include <span>

namespace vk { namespace handles {


BEGIN_DECLARE_VKSTRUCT(PipelineViewportStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineViewportStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, viewportCount)
    VKSTRUCT_PROPERTY(const VkViewport*, pViewports)
    VKSTRUCT_PROPERTY(uint32_t, scissorCount)
    VKSTRUCT_PROPERTY(const VkRect2D*, pScissors)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineDynamicStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineDynamicStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(uint32_t, dynamicStateCount)
    VKSTRUCT_PROPERTY(const VkDynamicState*, pDynamicStates)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineInputAssemblyStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineInputAssemblyStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkPrimitiveTopology, topology)
    VKSTRUCT_PROPERTY(VkBool32, primitiveRestartEnable)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineRasterizationStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineRasterizationStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkBool32, depthClampEnable)
    VKSTRUCT_PROPERTY(VkBool32, rasterizerDiscardEnable)
    VKSTRUCT_PROPERTY(VkPolygonMode, polygonMode)
    VKSTRUCT_PROPERTY(VkCullModeFlags, cullMode)
    VKSTRUCT_PROPERTY(VkFrontFace, frontFace)
    VKSTRUCT_PROPERTY(VkBool32, depthBiasEnable)
    VKSTRUCT_PROPERTY(float, depthBiasConstantFactor)
    VKSTRUCT_PROPERTY(float, depthBiasClamp)
    VKSTRUCT_PROPERTY(float, depthBiasSlopeFactor)
    VKSTRUCT_PROPERTY(float, lineWidth)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineMultisampleStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineMultisampleStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkSampleCountFlagBits, rasterizationSamples)
    VKSTRUCT_PROPERTY(VkBool32, sampleShadingEnable)
    VKSTRUCT_PROPERTY(float, minSampleShading)
    VKSTRUCT_PROPERTY(const VkSampleMask*, pSampleMask)
    VKSTRUCT_PROPERTY(VkBool32, alphaToCoverageEnable)
    VKSTRUCT_PROPERTY(VkBool32, alphaToOneEnable)
END_DECLARE_VKSTRUCT()


BEGIN_DECLARE_UNTYPED_VKSTRUCT(PipelineColorBlendAttachmentState)
    VKSTRUCT_PROPERTY(VkBool32, blendEnable)
    VKSTRUCT_PROPERTY(VkBlendFactor, srcColorBlendFactor)
    VKSTRUCT_PROPERTY(VkBlendFactor, dstColorBlendFactor)
    VKSTRUCT_PROPERTY(VkBlendOp, colorBlendOp)
    VKSTRUCT_PROPERTY(VkBlendFactor, srcAlphaBlendFactor)
    VKSTRUCT_PROPERTY(VkBlendFactor, dstAlphaBlendFactor)
    VKSTRUCT_PROPERTY(VkBlendOp, alphaBlendOp)
    VKSTRUCT_PROPERTY(VkColorComponentFlags, colorWriteMask)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineColorBlendStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineColorBlendStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkBool32, logicOpEnable)
    VKSTRUCT_PROPERTY(VkLogicOp, logicOp)
    VKSTRUCT_PROPERTY(uint32_t, attachmentCount)
    VKSTRUCT_PROPERTY(const VkPipelineColorBlendAttachmentState*, pAttachments)
    VKSTRUCT_PROPERTY(std::span<const float COMMA 4>, blendConstants)
END_DECLARE_VKSTRUCT()

BEGIN_DECLARE_VKSTRUCT(PipelineDepthStencilStateCreateInfo,
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO)
    VKSTRUCT_PROPERTY(const void*, pNext)
    VKSTRUCT_PROPERTY(VkPipelineDepthStencilStateCreateFlags, flags)
    VKSTRUCT_PROPERTY(VkBool32, depthTestEnable)
    VKSTRUCT_PROPERTY(VkBool32, depthWriteEnable)
    VKSTRUCT_PROPERTY(VkCompareOp, depthCompareOp)
    VKSTRUCT_PROPERTY(VkBool32, depthBoundsTestEnable)
    VKSTRUCT_PROPERTY(VkBool32, stencilTestEnable)
    VKSTRUCT_PROPERTY(VkStencilOpState, front)
    VKSTRUCT_PROPERTY(VkStencilOpState, back)
    VKSTRUCT_PROPERTY(float, minDepthBounds)
    VKSTRUCT_PROPERTY(float, maxDepthBounds)
END_DECLARE_VKSTRUCT()

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
