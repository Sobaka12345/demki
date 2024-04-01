#pragma once

#include "../operation_context.hpp"

namespace renderer {

namespace vk {
class StorageBuffer;
}

namespace ogl {
class StorageBuffer;
}


class IComputeTarget;

class ComputerInfoVisitor
{
public:
    virtual void populateComputerInfo(const vk::StorageBuffer& computeTarget)
    {
        ASSERT(false, "not implemented");
    };

    virtual void populateComputerInfo(const ogl::StorageBuffer& computeTarget)
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
    virtual OperationContext start(IComputeTarget& target) = 0;
    virtual void finish(OperationContext& target) = 0;

    virtual ~IComputer(){};
};

}    //  namespace renderer
