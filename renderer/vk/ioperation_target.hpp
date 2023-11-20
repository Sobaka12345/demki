#pragma once

#include <cstdint>

namespace vk {

class OperationContext;

class IOperationTarget
{
public:
    virtual ~IOperationTarget() {}

    virtual void populateWaitInfo(OperationContext& context) = 0;
    virtual void waitFor(OperationContext& context) = 0;
    virtual uint32_t descriptorsRequired() const = 0;
};

}    //  namespace vk
