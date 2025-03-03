#pragma once

#include "specific_pipeline.hpp"

#include "handles/compute_pipeline.hpp"

#include <icompute_pipeline.hpp>

namespace renderer::vk {

class ComputePipeline : public SpecificPipeline<IComputePipeline>
{
private:
    static ComputePipelineCreateInfo defaultPipeline();

public:
    ComputePipeline(GraphicsContext& context, CreateInfo createInfo);
    ~ComputePipeline();

    virtual ComputeDimensions computeDimensions() const override;
    virtual void bind(renderer::OperationContext& context) override;

private:
    VkComputePipeline pipeline(const OperationContext& context);

private:
    ComputeDimensions m_computeDimensions;
    handles::ComputePipeline::Map<VkRenderPass> m_pipelines;
};

}    //  namespace renderer::vk
