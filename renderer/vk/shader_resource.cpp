#include "shader_resource.hpp"

namespace vk {

std::shared_ptr<ShaderResource::Descriptor> ShaderResource::Descriptor::create(
    ShaderResource& shaderResource)
{
    return std::shared_ptr<ShaderResource::Descriptor>{ new ShaderResource::Descriptor(
        shaderResource) };
}

ShaderResource::Descriptor::Descriptor(ShaderResource& shaderResource)
    : shaderResource(shaderResource){};

ShaderResource::Descriptor::~Descriptor()
{
    shaderResource.freeDescriptor(*this);
}

bool ShaderResource::Descriptor::Id::operator<(const Id& other) const
{
    return resourceId < other.resourceId && bufferId < other.bufferId &&
        descriptorId < other.descriptorId;
}

bool ShaderResource::Descriptor::Id::operator==(const Id& other) const
{
    return resourceId == other.resourceId && bufferId == other.bufferId &&
        descriptorId == other.descriptorId;
}

std::shared_ptr<ShaderResource::Descriptor> ShaderResource::fetchDescriptor()
{
    return ShaderResource::Descriptor::create(*this);
}

void ShaderResource::freeDescriptor(const Descriptor& descriptor) {}

}    //  namespace vk
