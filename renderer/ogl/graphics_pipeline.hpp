#pragma once

#include <igraphics_pipeline.hpp>

namespace ogl {

class GraphicsContext;

class GraphicsPipeline : public IGraphicsPipeline
{
public:
    GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo);
    ~GraphicsPipeline();

    virtual void bind(::OperationContext& context) override;
    virtual FragileSharedPtr<IPipelineBindContext> bindContext(
        const IShaderInterfaceContainer& container) override;

private:
    const GraphicsContext& m_context;
};

}    //  namespace ogl
