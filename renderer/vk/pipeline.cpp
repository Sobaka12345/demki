#include "pipeline.hpp"

#include "graphics_context.hpp"
#include "uniform_handle.hpp"

#include "handles/command_buffer.hpp"
#include "handles/creators.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/pipeline_layout.hpp"
#include "handles/render_pass.hpp"
#include "handles/shader_module.hpp"

#include <boost/pfr.hpp>

#include <algorithm>

namespace vk {

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

VkShaderStageFlagBits toShaderStageFlags(IPipeline::ShaderType type)
{
    switch (type)
    {
        case UniformStage::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case UniformStage::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        default: ASSERT(false, "unknown shader type");
    }

    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

VkDescriptorType toDescriptorType(UniformType type)
{
    switch (type)
    {
        case UniformType::DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case UniformType::STATIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case UniformType::SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default: ASSERT(false, "unknown descriptor type");
    };

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

void Pipeline::BindContext::bind(::RenderContext& context, const IUniformContainer& container)
{
    auto& specContext = get(context);
    const auto uniforms = container.dynamicUniforms();

    std::vector<uint32_t> offsets(uniforms.size());
    std::transform(uniforms.begin(), uniforms.end(), offsets.begin(), [](const auto& descriptor) {
        DASSERT(!descriptor.handle.expired(), "handle is expired or has not been initialized");
        return descriptor.handle.lock()->resourceOffset();
    });

    specContext.commandBuffer->bindDescriptorSet(specContext.pipeline->layout(), setId, *set,
        offsets);
}

handles::GraphicsPipelineCreateInfo Pipeline::defaultPipeline()
{
    static constexpr std::array<VkDynamicState, 2> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    static constexpr handles::PipelineDynamicStateCreateInfo dynamicState =
        handles::PipelineDynamicStateCreateInfo()
            .dynamicStateCount(dynamicStates.size())
            .pDynamicStates(dynamicStates.data());

    static constexpr handles::PipelineViewportStateCreateInfo viewportState =
        handles::PipelineViewportStateCreateInfo().viewportCount(1).scissorCount(1);

    static constexpr handles::PipelineInputAssemblyStateCreateInfo inputAssembly =
        handles::PipelineInputAssemblyStateCreateInfo()
            .topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .primitiveRestartEnable(VK_FALSE);

    static constexpr handles::PipelineRasterizationStateCreateInfo rasterizer =
        handles::PipelineRasterizationStateCreateInfo()
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

    static constexpr handles::PipelineMultisampleStateCreateInfo multisampling =
        handles::PipelineMultisampleStateCreateInfo()
            .sampleShadingEnable(VK_FALSE)
            .rasterizationSamples(VK_SAMPLE_COUNT_1_BIT)
            .minSampleShading(1.0f)
            .pSampleMask(nullptr)
            .alphaToCoverageEnable(VK_FALSE)
            .alphaToOneEnable(VK_FALSE);

    static constexpr handles::PipelineDepthStencilStateCreateInfo depthStencil =
        handles::PipelineDepthStencilStateCreateInfo()
            .depthTestEnable(VK_TRUE)
            .depthWriteEnable(VK_TRUE)
            .depthCompareOp(VK_COMPARE_OP_LESS)
            .depthBoundsTestEnable(VK_FALSE)
            .stencilTestEnable(VK_FALSE);

    static constexpr std::array colorBlendAttachments = {
        handles::PipelineColorBlendAttachmentState()
            .colorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
            .colorBlendOp(VK_BLEND_OP_ADD)
            .srcColorBlendFactor(VK_BLEND_FACTOR_ONE)
            .dstColorBlendFactor(VK_BLEND_FACTOR_ZERO)
            .alphaBlendOp(VK_BLEND_OP_ADD)
            .srcAlphaBlendFactor(VK_BLEND_FACTOR_ONE)
            .dstAlphaBlendFactor(VK_BLEND_FACTOR_ZERO)
    };

    static constexpr handles::PipelineColorBlendStateCreateInfo colorBlending =
        handles::PipelineColorBlendStateCreateInfo()
            .attachmentCount(colorBlendAttachments.size())
            .pAttachments(colorBlendAttachments.data())
            .logicOp(VK_LOGIC_OP_COPY)
            .blendConstants({ 0.0f, 0.0f, 0.0f, 0.0f });

    return handles::GraphicsPipelineCreateInfo()
        .layout(VK_NULL_HANDLE)
        .renderPass(VK_NULL_HANDLE)
        .pStages(nullptr)
        .stageCount(0)
        .pVertexInputState(nullptr)
        .pInputAssemblyState(&inputAssembly)
        .pTessellationState(nullptr)
        .pViewportState(&viewportState)
        .pRasterizationState(&rasterizer)
        .pMultisampleState(&multisampling)
        .pColorBlendState(&colorBlending)
        .pDynamicState(&dynamicState)
        .pDepthStencilState(&depthStencil)
        .basePipelineHandle(VK_NULL_HANDLE)
        .basePipelineIndex(-1)
        .subpass(0)
        .flags(0)
        .pNext(nullptr);
}

Pipeline::Pipeline(const GraphicsContext& _context, IPipeline::CreateInfo createInfo)
    : m_context(_context)
    , m_createInfo(std::move(createInfo))
{
    m_bindingDescriptions.resize(m_createInfo.inputs.size());
    m_attributeDescriptions.reserve(m_createInfo.inputs.size() * 3);

    for (uint32_t i = 0; i < m_createInfo.inputs.size(); ++i)
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
            m_createInfo.inputs[i]);
    }

    std::vector<VkDescriptorSetLayout> layouts;
    std::vector<handles::DescriptorPoolSize> poolSizes;
    for (auto& set : m_createInfo.uniformSets)
    {
        std::vector<handles::DescriptorSetLayoutBinding> setLayoutBindings;
        auto span = set.second;
        for (auto& uniform : span)
        {
            const auto type = toDescriptorType(uniform.type);
            setLayoutBindings.push_back(
                handles::DescriptorSetLayoutBinding{}
                    .descriptorCount(uniform.count)
                    .binding(static_cast<uint32_t>(uniform.id))
                    .descriptorType(type)
                    .stageFlags(toShaderStageFlags(uniform.stage)));

            //  TO DO: push in set by types
            poolSizes.push_back(
                handles::DescriptorPoolSize{}.type(type).descriptorCount(uniform.count));
        }

        const auto& [iter, _] = m_setLayouts.emplace(set.first,
            std::pair{ layouts.size(),
                handles::DescriptorSetLayout(m_context.device(),
                    handles::DescriptorSetLayoutCreateInfo{}
                        .bindingCount(setLayoutBindings.size())
                        .pBindings(setLayoutBindings.data())) });
        layouts.push_back(iter->second.second);
    }

    m_pipelineLayout = std::make_unique<handles::PipelineLayout>(m_context.device(),
        handles::PipelineLayoutCreateInfo{}
            .setLayoutCount(layouts.size())
            .pSetLayouts(layouts.data())
            //  TO DO: push constants impl
            .pushConstantRangeCount(0));

    m_pool = std::make_unique<handles::DescriptorPool>(m_context.device(),
        handles::DescriptorPoolCreateInfo{}
            .maxSets(1000)
            .poolSizeCount(poolSizes.size())
            .pPoolSizes(poolSizes.data())
            .flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT));
}

Pipeline::~Pipeline() {}

void Pipeline::bind(::RenderContext& context)
{
    auto& specContext = get(context);
    specContext.commandBuffer->bindPipeline(pipeline(*specContext.renderPass));
    specContext.pipeline = this;
}

std::shared_ptr<IPipeline::IBindContext> Pipeline::bindContext(const IUniformContainer& container)
{
    std::span descriptors = container.uniforms();
    std::set<uint64_t> keySet;
    std::transform(descriptors.begin(), descriptors.end(), std::inserter(keySet, keySet.begin()),
        [](const auto& x) {
            DASSERT(!x.handle.expired(), "descriptor handle is expired or was not initialized");
            return x.handle.lock()->resource();
        });

    if (auto iter = m_bindContexts.find(keySet); iter != m_bindContexts.end())
    {
        return iter->second;
    }

    struct BufferInfoVisitor : public UniformHandleVisitor
    {
        void visit(UniformHandle& handle) override { result = &handle.descriptorBufferInfo; }

        handles::DescriptorBufferInfo* result;
    } bufferInfoVisitor;

    struct ImageInfoVisitor : public UniformHandleVisitor
    {
        void visit(UniformHandle& handle) override { result = &handle.descriptorImageInfo; }

        handles::DescriptorImageInfo* result;
    } imageInfoVisitor;

    auto& [layoutId, layout] = m_setLayouts.at(container.id());

    std::vector<handles::DescriptorSet::Write> writes;
    for (const auto& descriptor : descriptors)
    {
        if (descriptor.binding.type == UniformType::SAMPLER)
        {
            descriptor.handle.lock()->accept(imageInfoVisitor);
            writes.push_back(handles::DescriptorSet::Write{
                .imageInfo = *imageInfoVisitor.result,
                .layoutBinding = layout.binding(static_cast<int32_t>(descriptor.binding.id)),
            });
        }
        else
        {
            descriptor.handle.lock()->accept(bufferInfoVisitor);
            writes.push_back(handles::DescriptorSet::Write{
                .bufferInfo = *bufferInfoVisitor.result,
                .layoutBinding = layout.binding(static_cast<int32_t>(descriptor.binding.id)),
            });
        }
    }

    auto result = std::shared_ptr<BindContext>(new BindContext);
    result->set = m_pool->allocateSet(layout);
    result->set->write(writes);
    result->setId = layoutId;

    return result;
}

const handles::GraphicsPipeline& Pipeline::pipeline(const handles::RenderPass& renderPass)
{
    if (auto el = m_pipelines.find(&renderPass); el != m_pipelines.end())
    {
        return el->second;
    }

    handles::HandleVector<handles::ShaderModule> shaders;
    std::vector<handles::PipelineShaderStageCreateInfo> shaderStageCreateInfos;

    for (const auto& shaderInfo : m_createInfo.shaders)
    {
        shaders.emplace_back(m_context.device(), shaderInfo.path);
        shaderStageCreateInfos.push_back(
            handles::PipelineShaderStageCreateInfo{}
                .stage(toShaderStageFlags(shaderInfo.type))
                .module(shaders.back())
                .pName("main"));
    }

    const auto vertexInput =
        handles::PipelineVertexInputStateCreateInfo()
            .vertexBindingDescriptionCount(m_bindingDescriptions.size())
            .pVertexBindingDescriptions(m_bindingDescriptions.data())
            .vertexAttributeDescriptionCount(m_attributeDescriptions.size())
            .pVertexAttributeDescriptions(m_attributeDescriptions.data());

    auto [newEl, _] = m_pipelines.emplace(&renderPass,
        handles::GraphicsPipeline{ m_context.device(), VK_NULL_HANDLE,
            defaultPipeline()
                .renderPass(renderPass)
                .layout(*m_pipelineLayout)
                .stageCount(shaderStageCreateInfos.size())
                .pStages(shaderStageCreateInfos.data())
                .pVertexInputState(&vertexInput) });

    return newEl->second;
}

}    //  namespace vk
