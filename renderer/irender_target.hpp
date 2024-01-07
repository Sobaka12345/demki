#pragma once

#include "ioperation_target.hpp"

#include <cstdint>

class OperationContext;
class RenderInfoVisitor;

class IRenderTarget : public IOperationTarget
{
public:
    virtual ~IRenderTarget() {}

    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
    virtual void accept(RenderInfoVisitor& visitor) const = 0;
};
