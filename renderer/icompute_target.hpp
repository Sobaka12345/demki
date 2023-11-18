#pragma once

#include <cstdint>

class OperationContext;
class ComputerInfoVisitor;

class IComputeTarget
{
public:
    virtual ~IComputeTarget() {}

    virtual void accept(ComputerInfoVisitor& visitor) const = 0;
    [[nodiscard]] virtual bool prepare(OperationContext& context) = 0;
    virtual void compute(OperationContext& context) = 0;
};
