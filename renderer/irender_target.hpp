#pragma once

#include <cstdint>

class OperationContext;
class RenderInfoVisitor;

class IRenderTarget
{
public:
    virtual ~IRenderTarget() {}

    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
    virtual void accept(RenderInfoVisitor& visitor) const = 0;

    [[nodiscard]] virtual bool prepare(OperationContext& context) = 0;
    virtual void present(OperationContext& context) = 0;
};
