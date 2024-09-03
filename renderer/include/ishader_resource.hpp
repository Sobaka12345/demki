#pragma once

#include "ishader_interface_handle.hpp"

#include <iresource.hpp>

namespace renderer {

struct IShaderResource : public shell::IResource
{
    virtual void adapt(renderer::OperationContext& context, uint32_t bindingId) = 0;
    virtual void bind(renderer::OperationContext& context, uint32_t bindingId) const = 0;
};

}
