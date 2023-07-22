#pragma once

class RenderContext;
class RenderInfoVisitor;

class IRenderTarget
{
public:
    virtual ~IRenderTarget() {}

    virtual void accept(RenderInfoVisitor& visitor) const = 0;
    virtual void populateRenderContext(RenderContext& context) = 0;
};
