#include "graphics_pipeline.hpp"

#include "graphics_context.hpp"
#include "renderer.hpp"

#include "handles/command_buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/pipeline_layout.hpp"
#include "handles/render_pass.hpp"
#include "handles/shader_module.hpp"

#include <boost/pfr.hpp>

namespace {
template <typename T>
VkFormat attrubuteFormat(const T& attribute)
{
    if constexpr (std::is_same_v<T, float>)
    {
        return VK_FORMAT_R32_SFLOAT;
    }
    else if constexpr (std::is_same_v<T, glm::vec2>)
    {
        return VK_FORMAT_R32G32_SFLOAT;
    }
    else if constexpr (std::is_same_v<T, glm::vec3>)
    {
        return VK_FORMAT_R32G32B32_SFLOAT;
    }
    else if constexpr (std::is_same_v<T, glm::vec4>)
    {
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    return VK_FORMAT_UNDEFINED;
}
}    //  namespace

namespace vk {

void GraphicsPipeline::BindContext::bind(::OperationContext& context,
    const IShaderInterfaceContainer& container)
{
    auto& specContext = get(context);
    const auto uniforms = container.dynamicUniforms();
    std::vector<uint32_t> offsets(uniforms.size(), 0);
    std::for_each(uniforms.begin(), uniforms.end(), [](auto& descriptor) {
        DASSERT(!descriptor.handle.expired(), "handle is expired or has not been initialized");
        struct ResourceIdVisitor : public ShaderInterfaceHandleVisitor
        {
            void visit(ShaderInterfaceHandle& handle) override { handle.assureDescriptorCount(2); }
        } visitor;
        descriptor.handle.lock()->accept(visitor);
    });
    //    std::vector<uint32_t> offsets(uniforms.size());
    //    std::transform(uniforms.begin(), uniforms.end(), offsets.begin(), [](const auto&
    //    descriptor) {
    //        DASSERT(!descriptor.handle.expired(), "handle is expired or has not been
    //        initialized"); return descriptor.handle.lock()->resourceOffset();
    //    });

    //  TO DO: RETURN DYNAMIC OFFSETS

    specContext.commandBuffer->bindDescriptorSet(specContext.graphicsPipeline->layout(), setId,
        *set, offsets, VK_PIPELINE_BIND_POINT_GRAPHICS);
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
            .frontFace(VK_FRONT_FACE_CLOCKWISE)
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
    , m_shaders(std::move(createInfo.shaders()))
    , m_sampleShading(createInfo.sampleShading())
{
    m_bindingDescriptions.resize(createInfo.inputs().size());
    m_attributeDescriptions.reserve(createInfo.inputs().size() * 3);

    for (uint32_t i = 0; i < createInfo.inputs().size(); ++i)
    {
        std::visit(
            [&](auto& val) {
                m_bindingDescriptions[i].binding = i;
                m_bindingDescriptions[i].stride = sizeof(decltype(val));
                m_bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                uint32_t j = 0;
                boost::pfr::for_each_field(val, [&](auto& subVal) {
                    m_attributeDescriptions.push_back({
                        .location = j++,
                        .binding = i,
                        .format = attrubuteFormat(subVal),
                        .offset = static_cast<uint32_t>(
                            reinterpret_cast<int8_t*>(&subVal) - reinterpret_cast<int8_t*>(&val)),
                    });
                });
            },
            createInfo.inputs()[i]);
    }
}

GraphicsPipeline::~GraphicsPipeline() {}

void GraphicsPipeline::bind(::OperationContext& context)
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
        shaders.emplace_back(m_context.device(), shaderInfo.path);
        shaderStageCreateInfos.push_back(
            PipelineShaderStageCreateInfo{}
                .stage(toShaderStageFlags(shaderInfo.type))
                .module(shaders.back())
                .pName("main"));
    }

    PipelineMultisampleStateCreateInfo multisampling =
        PipelineMultisampleStateCreateInfo()
            .sampleShadingEnable(
                m_sampleShading == SampleShading::SS_0_PERCENT ? VK_FALSE : VK_TRUE)
            .rasterizationSamples(context.renderer->sampleCount())
            .minSampleShading(toVkSampleShadingCoefficient(m_sampleShading))
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
                .layout(*m_pipelineLayout)
                .stageCount(shaderStageCreateInfos.size())
                .pStages(shaderStageCreateInfos.data())
                .pVertexInputState(&vertexInput) });

    return newEl->second;
}

GraphicsPipeline::BindContext* GraphicsPipeline::newBindContext() const
{
    return new BindContext;
}

}    //  namespace vk
