#pragma once

#include <iswapchain.hpp>

namespace ogl {

class GraphicsContext;

class Swapchain : public ISwapchain
{
public:
    Swapchain(const GraphicsContext& context, ISwapchain::CreateInfo createInfo);
    ~Swapchain();

    virtual uint32_t width() const override;
    virtual uint32_t height() const override;
    virtual void accept(RenderInfoVisitor& visitor) const override;
    virtual bool prepare(OperationContext& context) override;
    virtual void present(OperationContext& context) override;

    virtual uint32_t framesInFlight() const override;


private:
    const GraphicsContext& m_context;
    std::pair<uint32_t, uint32_t> m_framebufferSize;
};

}    //  namespace ogl
