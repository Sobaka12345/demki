#pragma once

#include <cstdint>

namespace renderer {
class IOperationTarget;

namespace vk {

class OperationContext;

class ISpecificOperationTarget
{
public:
    virtual ~ISpecificOperationTarget() {}

    virtual IOperationTarget* toBase() = 0;

    virtual void populateWaitInfo(OperationContext& context) = 0;
    virtual void waitFor(OperationContext& context) = 0;
    virtual uint32_t descriptorsRequired() const = 0;
};

template <typename Base>
class SpecificOperationTarget
    : public ISpecificOperationTarget
    , public Base
{
    virtual Base* toBase() final override { return this; }
};

}    //  namespace vk
}    //  namespace renderer
