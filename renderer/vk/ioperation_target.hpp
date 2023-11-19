#pragma once

namespace vk {

class OperationContext;

class IOperationTarget
{
public:
    virtual ~IOperationTarget() {}

    virtual void populateWaitInfo(OperationContext& context) = 0;
    virtual void waitFor(OperationContext& context) = 0;
};

}
