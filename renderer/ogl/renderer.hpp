#pragma once

#include <irenderer.hpp>

#include <glad/glad.h>

namespace renderer::ogl {

class GraphicsContext;

class Renderer : public IRenderer
{
public:
    Renderer(const GraphicsContext& context, IRenderer::CreateInfo createInfo);

public:
    virtual renderer::OperationContext start(IRenderTarget& target) override;
    virtual void finish(renderer::OperationContext& context) override;

private:
    const GraphicsContext& m_context;
    glm::vec4 m_clearColor;
    Multisampling m_multisampling;

    struct SampleInfo
    {
        GLuint framebuffer;
        GLuint renderbuffer;
        GLuint texture;

        int width, height;
    };

    std::unordered_map<IOperationTarget*, SampleInfo> m_samples;
};

}    //  namespace renderer::ogl
