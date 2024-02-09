#pragma once

#include "pipeline.hpp"

#include "handles/graphics_pipeline.hpp"

#include <igraphics_pipeline.hpp>

namespace renderer::vk {

class GraphicsPipeline
    : public Pipeline
    , public IGraphicsPipeline
{
    struct BindContext : public Pipeline::BindContext
    {
        using Pipeline::BindContext::BindContext;

        virtual void bind(renderer::OperationContext& context,
            const IShaderInterfaceContainer& container) override;
    };

private:
    static GraphicsPipelineCreateInfo defaultPipeline();

public:
    GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo);
    ~GraphicsPipeline();

    virtual void bind(renderer::OperationContext& context) override;

private:
    const handles::Pipeline& pipeline(const vk::OperationContext& context);

    virtual BindContext* newBindContext(
        BindContext::DescriptorSetInfo descriptorSetInfo) const override;

private:
    VkCullModeFlags m_cullMode;
    VkFrontFace m_frontFace;
    VkPolygonMode m_polygonMode;
    VkPrimitiveTopology m_topology;
    glm::float32_t m_sampleShading;
    std::vector<ShaderInfo> m_shaders;
    std::map<const handles::RenderPass*, handles::GraphicsPipeline> m_pipelines;
};

}    //  namespace renderer::vk
