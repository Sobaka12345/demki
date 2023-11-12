#include "ubo_descriptor.hpp"

#include "vk/uniform_allocator.hpp"

namespace vk {

std::shared_ptr<UBODescriptor> UBODescriptor::create(UniformResource& uniformResource)
{
    return std::shared_ptr<UBODescriptor>{ new UBODescriptor(uniformResource) };
}

UBODescriptor::UBODescriptor(UniformResource& uniformResource)
    : uniformResource(uniformResource){};

UBODescriptor::~UBODescriptor()
{
    uniformResource.freeUBODescriptor(*this);
}

bool UBODescriptor::Id::operator<(const Id& other) const
{
    return resourceId < other.resourceId && bufferId < other.bufferId &&
        descriptorId < other.descriptorId;
}

bool UBODescriptor::Id::operator==(const Id& other) const
{
    return resourceId == other.resourceId && bufferId == other.bufferId &&
        descriptorId == other.descriptorId;
}

}    //  namespace vk
