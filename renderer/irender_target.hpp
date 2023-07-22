#pragma once

class RenderContext;
class RenderInfoVisitor;

class IRenderTarget
{
public:
    virtual ~IRenderTarget() {}

    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
    virtual void accept(RenderInfoVisitor& visitor) const = 0;
    virtual void populateRenderContext(RenderContext& context) = 0;
};
