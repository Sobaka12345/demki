#include "graphics_pipeline.hpp"

#include "graphics_context.hpp"
#include "renderer.hpp"
#include "descriptor_set_provider.hpp"

#include "handles/command_buffer.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/pipeline_layout.hpp"
#include "handles/render_pass.hpp"
#include "handles/shader_module.hpp"

#include <boost/pfr.hpp>

namespace renderer {

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

namespace vk {

void GraphicsPipeline::BindContext::bind(renderer::OperationContext& context,
    const IShaderInterfaceContainer& container)
{
    Pipeline::BindContext::bind(context, container);
    auto& specContext = get(context);

    std::vector<uint32_t> offsets;
    for (auto& descriptor : container.dynamicUniforms())
    {
        descriptor.handle.lock()->accept(s_handleVisitor);
        offsets.push_back(s_handleVisitor->currentDescriptor()->dynamicOffset);
    }

    specContext.commandBuffer->bindDescriptorSet(specContext.graphicsPipeline->layout(),
        descriptorSetInfo.setId, currentSet, offsets, VK_PIPELINE_BIND_POINT_GRAPHICS);
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

    static constexpr std::array colorBlendAttachments = {
        PipelineColorBlendAttachmentState()
            .colorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
            .colorBlendOp(VK_BLEND_OP_ADD)
            .srcColorBlendFactor(VK_BLEND_FACTOR_ONE)
            .dstColorBlendFactor(VK_BLEND_FACTOR_ZERO)
            .alphaBlendOp(VK_BLEND_OP_ADD)
            .srcAlphaBlendFactor(VK_BLEND_FACTOR_ONE)
            .dstAlphaBlendFactor(VK_BLEND_FACTOR_ZERO)
    };

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

GraphicsPipeline::GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo)
    : Pipeline(context, createInfo)
    , m_topology(toVkPrimitiveTopology(createInfo.primitiveTopology()))
    , m_shaders(std::move(createInfo.shaders()))
    , m_sampleShading(toVkSampleShadingCoefficient(createInfo.sampleShading()))
    , m_polygonMode(toVkPolygonMode(createInfo.polygonMode()))
    , m_cullMode(toVkCullMode(createInfo.cullMode()))
    , m_frontFace(toVkFrontFace(createInfo.frontFace()))
{
    m_bindingDescriptions.reserve(createInfo.bindings().size());
    m_attributeDescriptions.reserve(createInfo.attributes().size());

    for (const auto& bindingDesc : createInfo.bindings())
    {
        m_bindingDescriptions.push_back({
            .binding = bindingDesc.binding,
            .stride = bindingDesc.stride,
            .inputRate = toVkInputRate(bindingDesc.inputRate),
        });
    }

    for (const auto& attributeDesc : createInfo.attributes())
    {
        m_attributeDescriptions.push_back({
            .location = attributeDesc.location,
            .binding = attributeDesc.binding,
            .format = toVkAttrubuteFormat(attributeDesc.format),
            .offset = attributeDesc.offset,
        });
    }
}

GraphicsPipeline::~GraphicsPipeline() {}

void GraphicsPipeline::bind(renderer::OperationContext& context)
{
    auto& specContext = get(context);
    specContext.commandBuffer->bindPipeline(pipeline(specContext), VK_PIPELINE_BIND_POINT_GRAPHICS);
    specContext.graphicsPipeline = this;
}

const handles::Pipeline& GraphicsPipeline::pipeline(const OperationContext& context)
{
    if (auto el = m_pipelines.find(context.renderPass); el != m_pipelines.end())
    {
        return el->second;
    }

    handles::HandleVector<handles::ShaderModule> shaders;
    std::vector<PipelineShaderStageCreateInfo> shaderStageCreateInfos;

    for (const auto& shaderInfo : m_shaders)
    {
        shaders.emplaceBack(m_context.device(), shaderInfo.path);
        shaderStageCreateInfos.push_back(
            PipelineShaderStageCreateInfo{}
                .stage(toShaderStageFlags(shaderInfo.type))
                .module(shaders.back())
                .pName("main"));
    }

    PipelineRasterizationStateCreateInfo rasterizer =
        PipelineRasterizationStateCreateInfo()
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

    PipelineInputAssemblyStateCreateInfo inputAssembly =
        PipelineInputAssemblyStateCreateInfo()
            .topology(m_topology)
            .primitiveRestartEnable(VK_FALSE);

    PipelineMultisampleStateCreateInfo multisampling =
        PipelineMultisampleStateCreateInfo()
            .sampleShadingEnable(m_sampleShading < 0.01f ? VK_FALSE : VK_TRUE)
            .rasterizationSamples(context.renderer->sampleCount())
            .minSampleShading(m_sampleShading)
            .pSampleMask(nullptr)
            .alphaToCoverageEnable(VK_FALSE)
            .alphaToOneEnable(VK_FALSE);

    const auto vertexInput =
        PipelineVertexInputStateCreateInfo()
            .vertexBindingDescriptionCount(m_bindingDescriptions.size())
            .pVertexBindingDescriptions(m_bindingDescriptions.data())
            .vertexAttributeDescriptionCount(m_attributeDescriptions.size())
            .pVertexAttributeDescriptions(m_attributeDescriptions.data());

    auto [newEl, _] = m_pipelines.emplace(context.renderPass,
        handles::GraphicsPipeline{ m_context.device(), VK_NULL_HANDLE,
            defaultPipeline()
                .pMultisampleState(&multisampling)
                .renderPass(*context.renderPass)
                .pInputAssemblyState(&inputAssembly)
                .layout(*m_pipelineLayout)
                .stageCount(shaderStageCreateInfos.size())
                .pStages(shaderStageCreateInfos.data())
                .pVertexInputState(&vertexInput) });

    return newEl->second;
}

GraphicsPipeline::BindContext* GraphicsPipeline::newBindContext(
    BindContext::DescriptorSetInfo descriptorSetInfo) const
{
    return new BindContext(std::move(descriptorSetInfo));
}

}    //  namespace vk
}    //  namespace renderer
