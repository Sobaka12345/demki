#pragma once

#include "pipeline.hpp"

#include "handles/compute_pipeline.hpp"

namespace vk {

class ComputePipeline : public Pipeline
{
private:
    static ComputePipelineCreateInfo defaultPipeline();

public:
    ComputePipeline(const GraphicsContext& context, IPipeline::CreateInfo createInfo);
    ~ComputePipeline();

private:
    const handles::Pipeline& pipeline(const vk::RenderContext& context);

private:
    std::map<const handles::RenderPass*, handles::ComputePipeline> m_pipelines;
};

}    //  namespace vk
