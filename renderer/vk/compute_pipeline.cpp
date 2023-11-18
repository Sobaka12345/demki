#include "compute_pipeline.hpp"

#include "graphics_context.hpp"
#include "renderer.hpp"

#include "handles/command_buffer.hpp"
#include "handles/descriptor_pool.hpp"
#include "handles/descriptor_set.hpp"
#include "handles/descriptor_set_layout.hpp"
#include "handles/pipeline_layout.hpp"
#include "handles/render_pass.hpp"
#include "handles/shader_module.hpp"

namespace vk {

void ComputePipeline::BindContext::bind(::OperationContext& context,
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

    specContext.commandBuffer->bindDescriptorSet(specContext.computePipeline->layout(), setId, *set,
        offsets, VK_PIPELINE_BIND_POINT_COMPUTE);
}

ComputePipelineCreateInfo ComputePipeline::defaultPipeline()
{
    return ComputePipelineCreateInfo().layout(VK_NULL_HANDLE).flags(0).pNext(nullptr);
}

ComputePipeline::ComputePipeline(const GraphicsContext& context, CreateInfo createInfo)
    : Pipeline(context, createInfo)
    , m_shaders(std::move(createInfo.shaders()))
    , m_computeDimensions(createInfo.computeDimensions())
{}

ComputePipeline::~ComputePipeline() {}

IComputePipeline::ComputeDimensions ComputePipeline::computeDimensions() const
{
    return m_computeDimensions;
}

void ComputePipeline::bind(::OperationContext& context)
{
    auto& specContext = get(context);
    specContext.commandBuffer->bindPipeline(pipeline(specContext), VK_PIPELINE_BIND_POINT_COMPUTE);
    specContext.computePipeline = this;
}

const handles::Pipeline& ComputePipeline::pipeline(const OperationContext& context)
{
    if (auto el = m_pipelines.find(context.renderPass); el != m_pipelines.end())
    {
        return el->second;
    }

    ASSERT(m_shaders.size() == 1, "only compute shader is accepted");
    ASSERT(m_shaders.back().type == ShaderType::COMPUTE, "only compute shader is accepted");

    handles::ShaderModule shader{ m_context.device(), m_shaders.back().path };

    auto shaderStageCreateInfo =
        PipelineShaderStageCreateInfo{}
            .stage(toShaderStageFlags(ShaderType::COMPUTE))
            .module(shader)
            .pName("main");


    auto [newEl, _] = m_pipelines.emplace(context.renderPass,
        handles::ComputePipeline{ m_context.device(), VK_NULL_HANDLE,
            defaultPipeline().layout(*m_pipelineLayout).stage(shaderStageCreateInfo) });

    return newEl->second;
}

Pipeline::BindContext* ComputePipeline::newBindContext() const
{
    return new BindContext;
}

}    //  namespace vk
