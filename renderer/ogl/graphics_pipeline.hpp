#pragma once

#include "pipeline.hpp"

namespace renderer::ogl {

class GraphicsContext;

class GraphicsPipeline
    : public Pipeline
    , public IGraphicsPipeline
{
public:
    GraphicsPipeline(const GraphicsContext& context, CreateInfo createInfo);
    ~GraphicsPipeline();

    int primitiveTopology() const;

    virtual void bind(renderer::OperationContext& context) override;

private:
    glm::float32_t m_sampleShading;
    int m_primitiveTopology;
    int m_cullMode;
    int m_frontFace;
    int m_polygonMode;
};

}    //  namespace renderer::ogl
