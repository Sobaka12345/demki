#pragma once

#include "../utils.hpp"

#include "handles/descriptor_set.hpp"

#include "shader_interface_handle.hpp"

namespace renderer {
class IShaderResource;

namespace vk {

class OperationContext;

class ISpecificShaderResource
{
public:
    virtual ~ISpecificShaderResource() {}

    virtual IShaderResource* toBase() = 0;

protected:
    std::vector<handles::WriteDescriptorSet> descriptorSetWrites(
        renderer::vk::OperationContext& context,
        uint32_t bindingId,
        ShaderInterfaceHandle& handle) const;
};

template <typename IBase>
using SpecificShaderResource = SpecificBase<IBase, ISpecificShaderResource>;

}    //  namespace vk
}    //  namespace renderer
