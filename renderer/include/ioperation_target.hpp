#pragma once

namespace renderer {

class OperationContext;

struct IOperationTarget
{
    virtual ~IOperationTarget() {}

    [[nodiscard]] virtual bool prepare(OperationContext& context) = 0;
    virtual void present(OperationContext& context) = 0;
};

}
