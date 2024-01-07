#pragma once

#include "ioperation_target.hpp"

#include <cstdint>

class ComputerInfoVisitor;
class OperationContext;

class IComputeTarget : public IOperationTarget
{
public:
    virtual ~IComputeTarget() {}

    virtual void accept(ComputerInfoVisitor& visitor) const = 0;
};
