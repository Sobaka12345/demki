#pragma once

#include <cstdint>

class OperationContext;
class ComputerInfoVisitor;

class IComputeTarget
{
public:
    virtual ~IComputeTarget() {}

    virtual void accept(ComputerInfoVisitor& visitor) const = 0;
    virtual void populateOperationContext(OperationContext& context) = 0;
    [[nodiscard]] virtual bool prepare(OperationContext&) = 0;
    virtual void compute(OperationContext&) = 0;
};
