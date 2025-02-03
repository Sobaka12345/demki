#include "compute_pipeline.hpp"

#include "graphics_context.hpp"

#include <ishader_interface_container.hpp>

namespace renderer::vk {


ComputePipelineCreateInfo ComputePipeline::defaultPipeline()
{
    return ComputePipelineCreateInfo().layout(VK_NULL_HANDLE).flags(0).pNext(nullptr);
}

ComputePipeline::ComputePipeline(GraphicsContext& context, CreateInfo createInfo)
    : SpecificPipeline<IComputePipeline>(context)
    , m_computeDimensions(createInfo.computeDimensions())
{}

ComputePipeline::~ComputePipeline() {}

IComputePipeline::ComputeDimensions ComputePipeline::computeDimensions() const
{
    return m_computeDimensions;
}

void ComputePipeline::bind(renderer::OperationContext &context)
{
    auto& specContext = get(context);
    vkCmdBindPipeline(specContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline(specContext));
    specContext.computePipeline = this;
}

handles::ComputePipeline ComputePipeline::pipeline(const OperationContext& context)
{
    if (auto el = m_pipelines.find(context.renderPass); el != m_pipelines.end())
    {
        return el->second;
    }

    return VK_NULL_HANDLE;

    // handles::ShaderModule shader{ m_context.device(), m_shaders.back().path };

    // auto shaderStageCreateInfo =
    //     PipelineShaderStageCreateInfo{}
    //         .stage(toShaderStageFlags(ShaderStage::COMPUTE))
    //         .module(shader)
    //         .pName("main");


    // auto [newEl, _] = m_pipelines.emplace(context.renderPass,
    //     handles::ComputePipeline{ m_context.device(), VK_NULL_HANDLE,
    //         defaultPipeline().layout(*m_pipelineLayout).stage(shaderStageCreateInfo) });

    // return newEl->second;
}

}    //  namespace renderer::vk
