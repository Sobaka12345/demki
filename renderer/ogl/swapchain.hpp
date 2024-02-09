#pragma once

#include "ispecific_operation_target.hpp"

#include <iswapchain.hpp>

namespace renderer::ogl {

class GraphicsContext;

class Swapchain : public SpecificOperationTarget<ISwapchain>
{
public:
    Swapchain(GraphicsContext& context, ISwapchain::CreateInfo createInfo);
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
    std::pair<uint32_t, uint32_t> m_framebufferSize;
};

}    //  namespace renderer::ogl
