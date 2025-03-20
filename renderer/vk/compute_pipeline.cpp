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

GraphicsContext& ComputePipeline::context()
{
    return m_context;
}

IComputePipeline::ComputeDimensions ComputePipeline::computeDimensions() const
{
    return m_computeDimensions;
}

void ComputePipeline::bind(renderer::OperationContext& context)
{
    auto& specContext = get(context);
    vkCmdBindPipeline(specContext.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
        pipeline(specContext));
    specContext.computePipeline = this;
}

std::shared_ptr<IPipeline::Descriptor> ComputePipeline::spawnDescriptor()
{
    return std::make_shared<ISpecificPipelineDescriptor>(*this);
}

VkComputePipeline ComputePipeline::pipeline(const OperationContext& context)
{
    if (auto el = m_pipelines.find(context.renderPass); el != m_pipelines.end())
    {
        return el->second;
    }

    const auto pipelineCreateInfo =
        defaultPipeline().layout(m_pipelineLayout).stage(*m_shaderStageCreateInfos.data());

    VkComputePipeline pipeline = VK_NULL_HANDLE;
    handles::ComputePipeline::create(m_context.device(), VK_NULL_HANDLE, 1, &pipelineCreateInfo,
        nullptr, &pipeline);
    auto [newEl, inserted] = m_pipelines.emplace(context.renderPass, pipeline);
    DASSERT(inserted);

    return newEl->second;
}

}    //  namespace renderer::vk
