#pragma once

#include "ispecific_operation_target.hpp"

#include <iswapchain.hpp>
#include <utility>

namespace renderer {

class IOpenGLSurface;

namespace ogl {

class GraphicsContext;

class Swapchain : public SpecificOperationTarget<ISwapchain>
{
public:
    Swapchain(GraphicsContext& context, IOpenGLSurface& surface, ISwapchain::CreateInfo createInfo);
    ~Swapchain();

    virtual uint32_t width() const override;
    virtual uint32_t height() const override;
    virtual void accept(RenderInfoVisitor& visitor) const override;
    virtual bool prepare(renderer::OperationContext& context) override;
    virtual void present(renderer::OperationContext& context) override;

    virtual uint32_t framesInFlight() const override;

    virtual GLuint framebuffer() override;

private:
    GraphicsContext& m_context;
    IOpenGLSurface& m_surface;
    std::pair<uint32_t, uint32_t> m_framebufferSize;
};

}    //  namespace ogl
}    //  namespace renderer
