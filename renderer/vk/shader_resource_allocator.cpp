#include "shader_resource_allocator.hpp"

namespace renderer::vk {

std::shared_ptr<ShaderResourceAllocator::Descriptor> ShaderResourceAllocator::Descriptor::create(
    ShaderResourceAllocator& shaderResourceAllocator)
{
    return std::shared_ptr<ShaderResourceAllocator::Descriptor>{
        new ShaderResourceAllocator::Descriptor(shaderResourceAllocator)
    };
}

ShaderResourceAllocator::Descriptor::Descriptor(ShaderResourceAllocator& shaderResourceAllocator)
    : shaderResourceAllocator(shaderResourceAllocator) {};

ShaderResourceAllocator::Descriptor::~Descriptor()
{
    shaderResourceAllocator.freeDescriptor(*this);
}

bool ShaderResourceAllocator::Descriptor::Id::operator<(const Id& other) const
{
    return resourceId < other.resourceId && bufferId < other.bufferId &&
        descriptorId < other.descriptorId;
}

bool ShaderResourceAllocator::Descriptor::Id::operator==(const Id& other) const
{
    return resourceId == other.resourceId && bufferId == other.bufferId &&
        descriptorId == other.descriptorId;
}

std::shared_ptr<ShaderResourceAllocator::Descriptor> ShaderResourceAllocator::fetchDescriptor()
{
    return ShaderResourceAllocator::Descriptor::create(*this);
}

void ShaderResourceAllocator::freeDescriptor(const Descriptor& descriptor) {}

}    //  namespace renderer::vk
