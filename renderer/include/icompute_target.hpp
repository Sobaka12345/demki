#pragma once

#include <ioperation_target.hpp>

#include <cstdint>

namespace renderer {

class OperationContext;

class IComputeTarget : public IOperationTarget
{
public:
    virtual ~IComputeTarget() {}
};

}
