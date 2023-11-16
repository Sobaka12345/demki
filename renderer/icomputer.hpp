#pragma once

#include "operation_context.hpp"

class IComputeTarget;

namespace vk {
class ComputeTarget;
}

namespace ogl {
class ComputeTarget;
}

class ComputerInfoVisitor
{
public:
    virtual void populateComputerInfo(const vk::ComputeTarget& swapchain)
    {
        ASSERT(false, "not implemented");
    };

    virtual void populateRenderInfo(const ogl::ComputeTarget& swapchain)
    {
        ASSERT(false, "not implemented");
    };
};

class IComputer
{
public:
    struct CreateInfo
    {};

public:
    virtual IComputer& addComputeTarget(IComputeTarget& target) = 0;
    virtual OperationContext start(IComputeTarget& target) = 0;
    virtual void finish(OperationContext& target) = 0;

    virtual ~IComputer(){};
};
