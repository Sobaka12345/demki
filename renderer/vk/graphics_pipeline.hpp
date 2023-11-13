#pragma once

#include "pipeline.hpp"

#include "handles/graphics_pipeline.hpp"

namespace vk {

class GraphicsPipeline : public Pipeline
{
private:
    static GraphicsPipelineCreateInfo defaultPipeline();

public:
    GraphicsPipeline(const GraphicsContext& context, IPipeline::CreateInfo createInfo);
    ~GraphicsPipeline();

private:
    const handles::Pipeline& pipeline(const vk::RenderContext& context);

private:
    std::map<const handles::RenderPass*, handles::GraphicsPipeline> m_pipelines;
};

}    //  namespace vk
