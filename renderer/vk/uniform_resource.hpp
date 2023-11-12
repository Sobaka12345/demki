#pragma once

#include "iresource.hpp"

#include <memory>

namespace vk {

struct UBODescriptor;

class UniformResource : virtual public IResource
{
public:
    virtual std::shared_ptr<UBODescriptor> fetchUBODescriptor();

private:
    virtual void freeUBODescriptor(const UBODescriptor& descriptor) = 0;

    friend class UBODescriptor;
};

}
