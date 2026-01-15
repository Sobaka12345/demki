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
