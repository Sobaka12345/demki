#include "graphics_pipeline.hpp"

#include "../include/ishader_interface_container.hpp"

#include "handles/descriptor_set.hpp"

#include "graphics_context.hpp"
#include "renderer.hpp"
#include "specific_operation_target.hpp"

#include <boost/pfr.hpp>

namespace renderer::vk {

float toVkSampleShadingCoefficient(IGraphicsPipeline::CreateInfo::SampleShading sampleShading)
{
    switch (sampleShading)
    {
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_0_PERCENT: return 0.0f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_20_PERCENT: return 0.2f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_40_PERCENT: return 0.4f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_60_PERCENT: return 0.6f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_80_PERCENT: return 0.8f;
        case IGraphicsPipeline::CreateInfo::SampleShading::SS_100_PERCENT: return 1.0f;
    }

    ASSERT(false, "sample shading not declared");
    return 0.0f;
}

VkCullModeFlags toVkCullMode(IGraphicsPipeline::CreateInfo::CullMode cullMode)
{
    switch (cullMode)
    {
        case IGraphicsPipeline::CreateInfo::FRONT: return VK_CULL_MODE_FRONT_BIT;
        case IGraphicsPipeline::CreateInfo::BACK: return VK_CULL_MODE_BACK_BIT;
        case IGraphicsPipeline::CreateInfo::FRONT_AND_BACK: return VK_CULL_MODE_FRONT_AND_BACK;
    };
    ASSERT(false, "not implemented");
    return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
}

VkFrontFace toVkFrontFace(IGraphicsPipeline::CreateInfo::FrontFace frontFace)
{
    switch (frontFace)
    {
        case IGraphicsPipeline::CreateInfo::CLOCKWISE: return VK_FRONT_FACE_CLOCKWISE;
        case IGraphicsPipeline::CreateInfo::COUNTER_CLOCKWISE:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
    ASSERT(false, "not implemented");
    return VK_FRONT_FACE_MAX_ENUM;
}

VkPolygonMode toVkPolygonMode(IGraphicsPipeline::CreateInfo::PolygonMode polygonMode)
{
    switch (polygonMode)
    {
        case IGraphicsPipeline::CreateInfo::POINT: return VK_POLYGON_MODE_POINT;
        case IGraphicsPipeline::CreateInfo::LINE: return VK_POLYGON_MODE_LINE;
        case IGraphicsPipeline::CreateInfo::FILL: return VK_POLYGON_MODE_FILL;
    }
    ASSERT(false, "not implemented");
    return VK_POLYGON_MODE_MAX_ENUM;
}

VkPrimitiveTopology toVkPrimitiveTopology(IGraphicsPipeline::CreateInfo::PrimitiveTopology pt)
{
    switch (pt)
    {
        case IGraphicsPipeline::CreateInfo::POINTS: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case IGraphicsPipeline::CreateInfo::LINES: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case IGraphicsPipeline::CreateInfo::TRIANGLES: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
    ASSERT(false, "not implemented");
    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkVertexInputRate toVkInputRate(IGraphicsPipeline::CreateInfo::Binding::Rate rate)
{
    if (rate == IGraphicsPipeline::CreateInfo::Binding::VERTEX) return VK_VERTEX_INPUT_RATE_VERTEX;
    ASSERT(false, "not implemented");
    return VK_VERTEX_INPUT_RATE_MAX_ENUM;
}

VkFormat toVkAttrubuteFormat(IGraphicsPipeline::CreateInfo::Attribute::Format attribute)
{
    switch (attribute)
    {
        case IGraphicsPipeline::CreateInfo::Attribute::FLOAT: return VK_FORMAT_R32_SFLOAT;
        case IGraphicsPipeline::CreateInfo::Attribute::VEC2: return VK_FORMAT_R32G32_SFLOAT;
        case IGraphicsPipeline::CreateInfo::Attribute::VEC3: return VK_FORMAT_R32G32B32_SFLOAT;
        case IGraphicsPipeline::CreateInfo::Attribute::VEC4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default: return VK_FORMAT_UNDEFINED;
    };
}

GraphicsPipelineCreateInfo GraphicsPipeline::defaultPipeline()
{
    static constexpr std::array<VkDynamicState, 2> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    static constexpr PipelineDynamicStateCreateInfo dynamicState =
        PipelineDynamicStateCreateInfo()
            .dynamicStateCount(dynamicStates.size())
            .pDynamicStates(dynamicStates.data());

    static constexpr PipelineViewportStateCreateInfo viewportState =
        PipelineViewportStateCreateInfo().viewportCount(1).scissorCount(1);


    static constexpr PipelineInputAssemblyStateCreateInfo inputAssembly =
        PipelineInputAssemblyStateCreateInfo()
            .topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .primitiveRestartEnable(VK_FALSE);

    static constexpr PipelineRasterizationStateCreateInfo rasterizer =
        PipelineRasterizationStateCreateInfo()
            .depthClampEnable(VK_FALSE)
            .rasterizerDiscardEnable(VK_FALSE)
            .polygonMode(VK_POLYGON_MODE_FILL)
            .cullMode(VK_CULL_MODE_BACK_BIT)
            .frontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE)
            .depthBiasEnable(VK_FALSE)
            .lineWidth(1.0f)
            .depthBiasClamp(0.0f)
            .depthBiasConstantFactor(0.0f)
            .depthBiasSlopeFactor(0.0f);

    static constexpr PipelineDepthStencilStateCreateInfo depthStencil =
        PipelineDepthStencilStateCreateInfo()
            .depthTestEnable(VK_TRUE)
            .depthWriteEnable(VK_TRUE)
            .depthCompareOp(VK_COMPARE_OP_LESS)
            .depthBoundsTestEnable(VK_FALSE)
            .stencilTestEnable(VK_FALSE);

    static constexpr std::array colorBlendAttachments = { PipelineColorBlendAttachmentState()
            .colorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
            .colorBlendOp(VK_BLEND_OP_ADD)
            .srcColorBlendFactor(VK_BLEND_FACTOR_ONE)
            .dstColorBlendFactor(VK_BLEND_FACTOR_ZERO)
            .alphaBlendOp(VK_BLEND_OP_ADD)
            .srcAlphaBlendFactor(VK_BLEND_FACTOR_ONE)
            .dstAlphaBlendFactor(VK_BLEND_FACTOR_ZERO) };

    static constexpr std::array blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f };

    static constexpr PipelineColorBlendStateCreateInfo colorBlending =
        PipelineColorBlendStateCreateInfo()
            .attachmentCount(colorBlendAttachments.size())
            .pAttachments(colorBlendAttachments.data())
            .logicOp(VK_LOGIC_OP_COPY)
            .blendConstants(blendConstants);

    return GraphicsPipelineCreateInfo()
        .layout(VK_NULL_HANDLE)
        .renderPass(VK_NULL_HANDLE)
        .pStages(nullptr)
        .stageCount(0)
        .pVertexInputState(nullptr)
        .pInputAssemblyState(&inputAssembly)
        .pTessellationState(nullptr)
        .pViewportState(&viewportState)
        .pRasterizationState(&rasterizer)
        .pColorBlendState(&colorBlending)
        .pDynamicState(&dynamicState)
        .pDepthStencilState(&depthStencil)
        .basePipelineHandle(VK_NULL_HANDLE)
        .basePipelineIndex(-1)
        .subpass(0)
        .flags(0)
        .pNext(nullptr);
}

GraphicsPipeline::GraphicsPipeline(GraphicsContext& context, CreateInfo createInfo)
    : SpecificPipeline<IGraphicsPipeline>(context)
    , m_topology(toVkPrimitiveTopology(createInfo.primitiveTopology()))
    , m_sampleShading(toVkSampleShadingCoefficient(createInfo.sampleShading()))
    , m_polygonMode(toVkPolygonMode(createInfo.polygonMode()))
    , m_cullMode(toVkCullMode(createInfo.cullMode()))
    , m_frontFace(toVkFrontFace(createInfo.frontFace()))
{
    initModules(createInfo.shaderModules());

    m_rasterizationStateCreateInfo =
        PipelineRasterizationStateCreateInfo{}
            .depthClampEnable(VK_FALSE)
            .rasterizerDiscardEnable(VK_FALSE)
            .polygonMode(m_polygonMode)
            .cullMode(m_cullMode)
            .frontFace(m_frontFace)
            .depthBiasEnable(VK_FALSE)
            .lineWidth(1.0f)
            .depthBiasClamp(0.0f)
            .depthBiasConstantFactor(0.0f)
            .depthBiasSlopeFactor(0.0f);

    m_inputAssemblyCreateInfo =
        PipelineInputAssemblyStateCreateInfo()
            .topology(m_topology)
            .primitiveRestartEnable(VK_FALSE);

    m_multisampleStateCreateInfo =
        PipelineMultisampleStateCreateInfo{}
            .sampleShadingEnable(m_sampleShading < 0.01f ? VK_FALSE : VK_TRUE)
            .minSampleShading(m_sampleShading)
            .pSampleMask(nullptr)
            .alphaToCoverageEnable(VK_FALSE)
            .alphaToOneEnable(VK_FALSE);

    m_vertexInputStateCreateInfo =
        PipelineVertexInputStateCreateInfo{}
            .vertexAttributeDescriptionCount(0)
            .pVertexAttributeDescriptions(nullptr)
            .vertexBindingDescriptionCount(0)
            .pVertexBindingDescriptions(nullptr);
}

GraphicsPipeline::~GraphicsPipeline()
{
    m_pipelines.destroyAll(m_context.device());
}

GraphicsContext& GraphicsPipeline::context()
{
    return m_context;
}

void GraphicsPipeline::bind(renderer::OperationContext& context)
{
    auto& specContext = get(context);
    vkCmdBindPipeline(specContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline(specContext));
}

std::shared_ptr<IPipeline::Descriptor> GraphicsPipeline::spawnDescriptor()
{
    struct Descriptor : public ISpecificPipelineDescriptor
    {
        using ISpecificPipelineDescriptor::ISpecificPipelineDescriptor;

        virtual void bind(renderer::OperationContext& context) override
        {
            ISpecificPipelineDescriptor::bind(context);
            auto& specContext = get(context);
            vkCmdBindDescriptorSets(specContext.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout(), 0, descriptorSets.size(), descriptorSets.data(),
                dynamicOffsets.size(), dynamicOffsets.data());
        }
    };

    return std::make_shared<Descriptor>(*this);
}

VkGraphicsPipeline GraphicsPipeline::pipeline(const vk::OperationContext& context)
{
    if (auto el = m_pipelines.find(context.renderPass); el != m_pipelines.end())
    {
        return el->second;
    }

    m_multisampleStateCreateInfo.rasterizationSamples(context.renderer->sampleCount());

    const auto pipelineCreateInfo =
        defaultPipeline()
            .pMultisampleState(&m_multisampleStateCreateInfo)
            .renderPass(context.renderPass)
            .pInputAssemblyState(&m_inputAssemblyCreateInfo)
            .layout(m_pipelineLayout)
            .stageCount(m_shaderStageCreateInfos.size())
            .pStages(m_shaderStageCreateInfos.data())
            .pVertexInputState(&m_vertexInputStateCreateInfo);

    VkGraphicsPipeline pipeline = VK_NULL_HANDLE;
    handles::GraphicsPipeline::create(m_context.device(), VK_NULL_HANDLE, 1, &pipelineCreateInfo,
        nullptr, &pipeline);
    auto [newEl, inserted] = m_pipelines.emplace(context.renderPass, pipeline);
    DASSERT(inserted);

    return newEl->second;
}

}    //  namespace renderer::vk
