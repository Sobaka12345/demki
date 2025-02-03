#pragma once

#include "specific_pipeline.hpp"

#include "handles/compute_pipeline.hpp"

#include <icompute_pipeline.hpp>

namespace renderer::vk {

class ComputePipeline: public SpecificPipeline<IComputePipeline>
{
private:
    static ComputePipelineCreateInfo defaultPipeline();

public:
    ComputePipeline(GraphicsContext& context, CreateInfo createInfo);
    ~ComputePipeline();

    virtual ComputeDimensions computeDimensions() const override;
    virtual void bind(renderer::OperationContext& context) override;

private:
    handles::ComputePipeline pipeline(const OperationContext& context);

private:
    std::vector<ShaderInfo> m_shaders;
    ComputeDimensions m_computeDimensions;
    handles::ComputePipelineContainer::Map<handles::RenderPass> m_pipelines;
};

}    //  namespace renderer::vk
