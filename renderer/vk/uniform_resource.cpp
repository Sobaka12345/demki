#include "uniform_resource.hpp"

#include "ubo_descriptor.hpp"

namespace vk {

std::shared_ptr<UBODescriptor> UniformResource::fetchUBODescriptor()
{
    return UBODescriptor::create(*this);
}

}
