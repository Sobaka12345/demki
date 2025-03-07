#pragma once

#include <ioperation_target.hpp>

#include <cstdint>

namespace renderer {

class ComputerInfoVisitor;
class OperationContext;

class IComputeTarget : public IOperationTarget
{
public:
    virtual ~IComputeTarget() {}
};

}
