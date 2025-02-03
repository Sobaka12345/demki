#pragma once

#include <iresource.hpp>

namespace renderer {

class OperationContext;

struct IShaderResource : public shell::IResource
{
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const = 0;
};

}
