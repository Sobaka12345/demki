#pragma once

#include <iresource.hpp>

namespace renderer {

class OperationContext;

struct IShaderResource : public resources::IResource
{
    virtual void init(renderer::OperationContext& context, uint32_t bindingId) const = 0;
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const = 0;

protected:
    using resources::IResource::IResource;
};

}
