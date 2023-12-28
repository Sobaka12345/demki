#pragma once

#include <irenderer.hpp>

namespace ogl {

class GraphicsContext;

class Renderer : public IRenderer
{
public:
    Renderer(const GraphicsContext& context, IRenderer::CreateInfo createInfo);

public:
    virtual ::OperationContext start(IRenderTarget& target) override;
    virtual void finish(::OperationContext& context) override;

private:
    const GraphicsContext& m_context;
};

}    //  namespace ogl
