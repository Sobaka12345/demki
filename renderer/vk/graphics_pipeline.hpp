#pragma once

#include "specific_pipeline.hpp"

#include "handles/graphics_pipeline.hpp"
#include "handles/pipeline_layout.hpp"

#include <igraphics_pipeline.hpp>

namespace renderer::vk {

class GraphicsPipeline : public SpecificPipeline<IGraphicsPipeline>
{
private:
    static GraphicsPipelineCreateInfo defaultPipeline();

public:
    GraphicsPipeline(GraphicsContext& context, CreateInfo createInfo);
    ~GraphicsPipeline();

    virtual GraphicsContext& context() override;

    virtual void bind(renderer::OperationContext& context) override;
    virtual std::shared_ptr<IPipeline::Descriptor> spawnDescriptor() override;

private:
    VkGraphicsPipeline pipeline(const vk::OperationContext& context);

private:
    VkCullModeFlags m_cullMode;
    VkFrontFace m_frontFace;
    VkPolygonMode m_polygonMode;
    VkPrimitiveTopology m_topology;
    glm::float32_t m_sampleShading;

    PipelineRasterizationStateCreateInfo m_rasterizationStateCreateInfo;
    PipelineInputAssemblyStateCreateInfo m_inputAssemblyCreateInfo;
    PipelineMultisampleStateCreateInfo m_multisampleStateCreateInfo;
    PipelineVertexInputStateCreateInfo m_vertexInputStateCreateInfo;

    handles::GraphicsPipeline::Map<VkRenderPass> m_pipelines;
};

}    //  namespace renderer::vk
