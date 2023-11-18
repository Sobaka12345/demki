#pragma once

#include "pipeline.hpp"

#include "handles/graphics_pipeline.hpp"

#include <igraphics_pipeline.hpp>

namespace vk {

class GraphicsPipeline
    : public Pipeline
    , public IGraphicsPipeline
{
    struct BindContext : public Pipeline::BindContext
    {
        virtual void bind(::OperationContext& context,
            const IShaderInterfaceContainer& container) override;
    };

private:
    static GraphicsPipelineCreateInfo defaultPipeline();

public:
    GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo);
    ~GraphicsPipeline();

    virtual void bind(::OperationContext& context) override;

private:
    const handles::Pipeline& pipeline(const vk::OperationContext& context);

    virtual BindContext* newBindContext() const override;

private:
    SampleShading m_sampleShading;
    std::vector<ShaderInfo> m_shaders;
    std::map<const handles::RenderPass*, handles::GraphicsPipeline> m_pipelines;
};

}    //  namespace vk
