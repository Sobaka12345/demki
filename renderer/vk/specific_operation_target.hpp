#pragma once

#include <cstdint>

#include "../utils.hpp"

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

template <typename IBase>
using SpecificOperationTarget = SpecificBase<IBase, ISpecificOperationTarget>;

}    //  namespace vk
}    //  namespace renderer
